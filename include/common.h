#pragma once
#include <windows.h>
#include <string>
template <typename Component, typename Param>
class ModuleLoader
{
    typedef typename Component* (__stdcall *Function)(Param);
    class FileLoader
    {
    public:
        FileLoader(const char* pFileName)
            : m_fileName(pFileName)
        {
            m_fileHandle = LoadLibraryA(m_fileName.c_str());
        }

        ~FileLoader()
        {
            unload();
        }

        void unload()
        {
            if (m_fileHandle)
            {
                FreeLibrary(m_fileHandle);
                m_fileHandle = NULL;
            }
        }

        bool loadManually()
        {
            if (!m_fileHandle)
                m_fileHandle = LoadLibraryA(m_fileName.c_str());

            return !!m_fileHandle;
        }

        HINSTANCE handle() const
        {
            return m_fileHandle;
        }
    protected:
        HINSTANCE m_fileHandle;
        std::string m_fileName;
    };

public:
    ModuleLoader(const char* pDllPath, const char* CreateFunctionName)
        : m_fileLoader(pDllPath)
        , m_pCreateFunc(nullptr)
        , m_pCreateFuncName(CreateFunctionName)
    {
        loadFuncManually();
    }

    Component* createComponent(Param param)
    {
        loadManually();
        if (m_pCreateFunc)
            return m_pCreateFunc(param);

        return nullptr;
    }

    void loadManually()
    {
        m_fileLoader.loadManually();
        loadFuncManually();
    }

    void unload()
    {
        m_fileLoader.unload();
        m_pCreateFunc = NULL;
    }

protected:
    void loadFuncManually()
    {
        if (m_pCreateFunc == nullptr)
        {
            if (m_fileLoader.handle())
                m_pCreateFunc = (Function)GetProcAddress(m_fileLoader.handle(), m_pCreateFuncName);
            else
                m_pCreateFunc = nullptr;
        }
    }

protected:
    FileLoader m_fileLoader;
    Function m_pCreateFunc;
    const char* m_pCreateFuncName;
};
