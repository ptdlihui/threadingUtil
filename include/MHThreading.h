#pragma once
#include "parameters.h"
#include <thread>
#include <mutex>
#include <condition_variable>
#include "lockedcontainer.h"
#include <list>
#include "refcounted.h"

class MHThreadingTask : public MHRefCounted
{
public:
    MHThreadingTask() : m_bSync(false), m_tsComplete(false) {};
    virtual ~MHThreadingTask() {};

    void execute()
    {
        if (m_bSync)
        {
            Run();
            m_tsComplete.updateContent(true, true);
            m_syncNotifier.notify_one();
        }
        else
            Run();
    }

    void waitForComplete()
    {
        std::unique_lock<std::mutex> lk(m_mtx);

        m_tsComplete.Lock();
        bool bComplete = m_tsComplete.checkout();
        m_tsComplete.UnLock();

        while (!bComplete)
        {
            if (!bComplete)
                m_syncNotifier.wait(lk);

            m_tsComplete.Lock();
            bComplete = m_tsComplete.checkout();
            m_tsComplete.UnLock();
        };
    }

    void setSyncAble(bool bSync)
    {
        m_bSync = bSync;
    }

    virtual void Run() = 0;

protected:
    bool m_bSync;
    LockedContainer<bool> m_tsComplete;
    std::mutex m_mtx;
    std::condition_variable m_syncNotifier;
};


typedef MHSharedPtr<MHThreadingTask> ThreadingTaskPointer;

typedef std::list<ThreadingTaskPointer> TaskList;

class ThreadingTaskQueue
{
public:
    ThreadingTaskPointer waitAndPop()
    {
        std::unique_lock<std::mutex> lock(m_mtx);

        while (m_taskQueue.empty())
            m_hasTaskNotifier.wait(lock);

        ThreadingTaskPointer result = m_taskQueue.front();
        m_taskQueue.pop_front();
        
        return result;
    }

    void pushTask(ThreadingTaskPointer spTask)
    {
        std::unique_lock<std::mutex> lock(m_mtx);

        bool isEmpty = m_taskQueue.empty();

        m_taskQueue.push_back(spTask);

        lock.unlock();

        if (isEmpty)
            m_hasTaskNotifier.notify_one();

    }

protected:
    TaskList m_taskQueue;
    std::mutex m_mtx;
    std::condition_variable m_hasTaskNotifier;
};


class TaskThreadEntry
{
public:
    TaskThreadEntry(ThreadingTaskQueue& queue)
        : m_queue(queue)
    {
    }

    void operator() ()
    {
        for (;;)
        {
            ThreadingTaskPointer spTask = m_queue.waitAndPop();

            if (spTask.get())
                spTask->execute();
            else
                break;
        }
    }
protected:
    ThreadingTaskQueue& m_queue;
};

class MHThread
{
public:
    MHThread() : m_pThread(nullptr)
        , m_threadingEntry(m_taskQueue)
    {}

    virtual ~MHThread()
    {
        FinishThread();
    }
    

    void StartThread()
    {
        if (m_pThread)
            FinishThread();

        m_pThread = new std::thread(m_threadingEntry);
    }

    void FinishThread()
    {
        if (m_pThread)
        {
            ThreadingTaskPointer spTask(nullptr);
            m_taskQueue.pushTask(spTask);
            m_pThread->join();
            delete m_pThread;
            m_pThread = nullptr;
        }
    }

    void PushTask(ThreadingTaskPointer spTask)
    {
        m_taskQueue.pushTask(spTask);
    }

    bool ThreadStarted()
    {
        return !!m_pThread;
    }

protected:
    ThreadingTaskQueue m_taskQueue;
    TaskThreadEntry m_threadingEntry;
    std::thread* m_pThread;
};

template <typename T>
class LockedSharedPtr : public LockedContainer<MHSharedPtr<T>>
{
};




