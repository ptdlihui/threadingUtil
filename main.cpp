#include <iostream>
#include "MHThreading.h"
#include <vector>


class TestThread : public MHThread
{

};


class TestTask : public MHThreadingTask
{
public:
    TestTask(std::vector<int>& container, int input, bool sync = false)
        : m_container(container)
        , m_input(input)
    {
        if (sync)
            setSyncAble(true);
    }

    virtual void Run() override
    {
        m_container.push_back(m_input);
    }
protected:
    std::vector<int>& m_container;
    int m_input;
};
int main()
{
    std::vector<int> testContainer;
    TestThread thread;

    thread.StartThread();
    TestTask* pTask = new TestTask(testContainer, 0);
    thread.PushTask(pTask);

    ThreadingTaskPointer spTask = new TestTask(testContainer, 1, true);
    thread.PushTask(spTask);
    spTask->waitForComplete();

    return 0;
}