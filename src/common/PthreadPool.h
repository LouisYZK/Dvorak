#ifndef PTHREAD_POOL_H
#define PTHREAD_POOL_H

#include <unistd.h>
#include <string>
#include <list>
#include <vector>
#include <pthread.h>
#include <time.h>
#include <future>
#include <functional>
#include "Logger.h"

using std::list;
using std::vector;

const static timespec ts = {0, 1};

class PthreadPool;

typedef const void* (*ProcessFunc)(const void *);
typedef void (*CallbackFunc)(void*, const void*);
class ThreadTask
{
    private:
        friend class PthreadPool;
        ProcessFunc m_ProcessFunc;
        CallbackFunc m_CallbackFunc;
        const void * m_Arg;
        const void * m_Res;
        void* m_CallbackArg;

    protected:
        void* m_pData;
        inline void initTask()
        {
            m_bFinished = false;
            m_pData = nullptr;
        }
    
    public:
        bool m_bFinished;

        ThreadTask(ProcessFunc process, const void* arg, 
                   CallbackFunc callback, void* call_arg)
            : m_ProcessFunc(process),
              m_Arg(arg),
              m_CallbackFunc(callback),
              m_CallbackArg(call_arg)
        {
            initTask();
        }

        virtual ~ThreadTask()
        {
            initTask();
        }

        void Run()
        {
            m_Res = m_ProcessFunc(m_Arg);
            m_CallbackFunc(m_CallbackArg, m_Res);   
        }
        
        const void* Result()
        {
            if (WaitTask() == 0)
            {
                return m_Res;
            }
            return NULL;
        }

        inline void SetData(void* data)
        {
            m_pData = data;
        }
        
        inline int WaitTask()
        {
            if ( !m_bFinished )
            {
                while ( !m_bFinished )
                {
                    nanosleep(&ts, NULL);
                }
            }
            return 0;
        }
        
};

class PthreadPool
{
    public:
        list<ThreadTask*> m_vTaskList;

        // To synchronize the shared var (usually as condition variable)
        pthread_mutex_t m_pthreadMutex;
        // synchronize condition signal
        pthread_cond_t m_pthreadCond;

        bool m_bDestroyAll;
        vector<pthread_t> m_vThreadID;

    protected:
        void DestroyAllThread();
        void CreateAllThread(int threadNum);

    public:
        static void* ThreadFunc(void* threadData);

        PthreadPool(int threadNum);
        virtual ~PthreadPool();
        
        void AddTask(ThreadTask* t);
        int GetTaskSize();
};

#endif