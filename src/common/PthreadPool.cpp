#include "PthreadPool.h"

PthreadPool::PthreadPool(int threadNum)
{
    m_bDestroyAll = false;
    CreateAllThread(threadNum);
}

PthreadPool::~PthreadPool()
{
    console_info("~PthreadPool");
    DestroyAllThread();
}

void* PthreadPool::ThreadFunc(void* threadData)
{
    if ( threadData == nullptr )
    {
        console_error("Thread data is null");
        return NULL;
    }
    PthreadPool* PoolData = (PthreadPool*)threadData;
    
    //loop for wait tasks
    while (true)
    {
        ThreadTask* pTask = nullptr;
        pthread_mutex_lock(&PoolData->m_pthreadMutex);
        while ( PoolData->m_vTaskList.empty() && !PoolData->m_bDestroyAll)
        {
            pthread_cond_wait(&PoolData->m_pthreadCond, &PoolData->m_pthreadMutex);
        }
        if ( !PoolData->m_vTaskList.empty() )
        {
            auto iter = PoolData->m_vTaskList.begin();
            pTask = *iter;
            PoolData->m_vTaskList.erase(iter);
        }
        pthread_mutex_unlock(&PoolData->m_pthreadMutex);
        
        if ( pTask != nullptr )
        {
            pTask->Run();
            pTask->m_bFinished = true;
        }

        if ( PoolData->m_bDestroyAll && PoolData->m_vTaskList.empty() )
        {
            console_info("Thread exited");
            pthread_exit(NULL);
            break;
        }
    }
    return NULL;
}

void PthreadPool::AddTask(ThreadTask* t)
{
    pthread_mutex_lock(&m_pthreadMutex);
    m_vTaskList.push_back(t);
    pthread_cond_signal(&m_pthreadCond);
    pthread_mutex_unlock(&m_pthreadMutex);
}

void PthreadPool::CreateAllThread(int threadNum)
{
    pthread_mutex_init(&m_pthreadMutex, NULL);
    pthread_cond_init(&m_pthreadCond, NULL);
    m_vThreadID.resize(threadNum);
    
    for ( int i = 0; i < threadNum; ++i)
    {
        pthread_create(&m_vThreadID[i], NULL, ThreadFunc, (void *)this);
        console_info("Thread {} has start!", i);
    }
}

void PthreadPool::DestroyAllThread()
{
    if ( !m_bDestroyAll )
    {
        m_bDestroyAll = true;
        console_info("Start to destroy");
        pthread_cond_broadcast(&m_pthreadCond);
        for ( int i = 0; i < m_vThreadID.size(); ++i)
        {
            pthread_join(m_vThreadID[i], NULL);
            console_info("Destroy thread {}", i);
        }
        m_vThreadID.clear();

        pthread_cond_destroy(&m_pthreadCond);
        pthread_mutex_destroy(&m_pthreadMutex);
    }
}

int PthreadPool::GetTaskSize()
{
    return m_vThreadID.size();
}



