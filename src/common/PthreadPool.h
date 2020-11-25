#ifndef PTHREAD_POOL_H
#define PTHREAD_POOL_H

#include <unistd.h>
#include <string>
#include <list>
#include <vector>
#include <pthread.h>
#include <time.h>
#include "Logger.h"

using std::list;
using std::vector;

const static timespec ts = {0, 1};

class PthreadPool;

class ThreadTask
{
    private:
        friend class PthreadPool;  

    protected:
        void* m_pData;
        inline void initTask()
        {
            m_bFinished = false;
            m_pData = nullptr;
        }
    
    public:
        bool m_bFinished;
        ThreadTask()
        {
            initTask();
        }
        virtual ~ThreadTask()
        {
            initTask();
        }

        virtual void Run() = 0;
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