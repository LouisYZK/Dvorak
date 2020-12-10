#! https://zhuanlan.zhihu.com/p/321319234
# [C++造轮子] 基于pthread线程池
- [原文地址](http://zhikai.pro/post/103)
- [本文完整代码](https://github.com/LouisYZK/Dvorak/blob/master/src/common/PthreadPool.cpp)

> 本篇是C++轮子系列的第二个主题，之前第一个主题为对象序列化和数据加载相关。C++工程很多基础模块需要自己封装，常见的线程池是其中一项。线程池在IO密集、并发任务、异步任务、后台监控任务场景中都发挥作用。而`线程`话题无论是操作系统和高级编程语言上都是一个复杂的话题。本文的讨论是基于Posix标准讨论的。

> 题外话，作为一名pythonista, 每当听到身边同行对python多线程的吐槽都十分无语，他们大多不清楚GIL出现的原因、适合什么场景、现在PEP给出了哪些解决方案、是否其他的python实现规避了GIL等等问题。只是单纯地知道python的`伪多线程`。使用cpython至今，仍然没找到需要语言层面出面解决计算密集任务的场景，因各类计算框架没有一个只用python原生实现。而web中单纯的io场景无论是协程、多线程、第三方事件循环都解决的很好。

本文的主要内容有：

- Linux线程与内核支持
- posix标准与pthread库
- 基于pthread库的线程池实现

## Linux线程与内核支持
### Process, Thread 与 Task
首先我们基于Unix和Posix线程标准谈一下这三个概念：
- Process: 广义上就是运行的程序，细节上看就是一段地址空间，存放各类指令、数据和其他信息。放在Posix标准下看就有更明确的结构体定义。
- thread: Posix标准定义被应用识别执行的单一路径就是线程。进程包含多个线程。
- task: Linux基础系统调用`do_fork`创建出的基本调度单元

这三个概念有区别和联系，甚至还有`内核线程`、`用户线程`、`协程`、`轻量级进程LWP`的概念，这些概念的出现以及线程的发展历史又是另外的故事了。本文在这里还是简单地对他们做一区分，虽然你会在不同的评论区看到对这些概念的理解大相径庭。

- 由内核管理调度的是内核线程，由用户模拟调度的是用户线程，这么讲用户线程只有协程一种
- 主要运行在内核态的为内核线程，主要运行在用户态的是用户线程。(这个分发我个人不赞同)
- 用户创建线程时也在内核里创建一个调度实体(kernel scheduling entity)与之对应，由内核线程配合调度用户线程，这种模型成为LWP

因为论坛内对上述概念有不同的表述所以在问题 `pthread实现的究竟是内核线程还是用户线程？`有不同的答案。

### 内核的线程支持
如果一个概念的提出能得到操作系统内核的原生支持是最直接的解决方案，但在没有线程出现前，进程或者说task的调度机制很成熟了。如何在现有的系统调用基础上支持线程的讨论一直进行。

Linux的资源调度和cpu运行调度的最小单位都是`task_struct`,  创建时最终都会调用`do_fork`函数，不同之处是传入的参数不同（`clone_flags`），最终结果就是进程有独立的地址空间和栈，而用户线程可以自己指定用户栈，地址空间和父进程共享，内核线程则只有和内核共享的同一个栈，同一个地址空间。当然不管是进程还是线程，`do_fork`最终会创建一个`task_struct`结构。 还有一个系统调用`kthread_create`专门用来创建内核线程，虽然最终也是调用`do_fork`，区别在于内核线程共用内核空间、可以指定回到函数。

当然用户不可能一直使用系统调用创建线程，一个线程库是很有必要的，前提是库的接口都要符合Posix标准对于线程的描述，比如著名的`1:1`理论模型。


### Posix线程历史
Posix线程标准不开源且庞大，但我们无需知其细节。只需要知道一个通用的线程标准对多种系统、架构和语言来讲是必要的。基于Posix线程标准的库就知道其采用了1:1的模型，如C的`pthread`。C++的`thread`是基于`boost::thread`，他是封装`pthread`实现的。

> 什么是1:1调度模型？ 类似的还有N:1, M:N。在上文中对线程的用户与内核分类中讲到，LWP的实现是由内核配合调度一个用户线程。M:N模型就是由N个内核线程配合调度用户线程。在LWP语境下，很难讲明白`pthread`到底是用户线程还是内核线程。当然也没必要拘泥于这个概念。

> 关于Linux内核的进程和线程，实际上由于历史原因，Linux也遵守了真香定律。以pthread的两种实现LinuxThread（Linux2.4以前）和NPTL（2.5以后）为例，早期内核是没有TGID的，只有PID，所以内核根本就没有线程的概念，不管有没有CLONE_VM，在内核眼里都是进程，都是调度器调度的单元，而这就给LinuxThread的实现带来了难题——LinuxThread采用1:1模型，即每个线程都是LWP对应一个内核线程（这个线程的概念是从我们的视角出发的），在内核不支持的情况下如何实现线程的一套同步互斥呢？LinuxThread使用信号来模拟，显然效率不高。而NPTL时代，内核引入TGID，此时依然是1:1模型，但不是简单的LWP了，TGID把这些线程联系了起来，那如何告知内核呢？答案就是NPTL创建线程时传递一个CLONE_THREAD 标志，所以用的是clone。内核把TGID填写为调用者的PID，PID填写新线程号（原本的进程号）。有了这一层联系，线程的同步互斥就可以依赖于用户空间的锁（因为大家共享同一片地址空间），省去了此前内核态切来切去的麻烦，效率自然高了，也就是所谓的futex。[2]

![](https://pic.downk.cc/item/5fc35fc1d590d4788abe08d5.jpg)

不是每个用户都知晓内核的逻辑，`pthread`只要干一件事，让用户知道自己通过库函数创建的是真的可以多核并行的线程即可。至于他的与内核的策略、同步策略的实现上可以不关心。

## 基于pthread(NTPL)实现线程池基本功能
本节就基于NPTL版本的pthread实现一个线程池，线程池可以节省线程频繁创建、销毁、调度的开销。关于pthread的基本接口可以参考[1].

![](https://pic.downk.cc/item/5fc372aed590d4788ac62439.jpg)

### 任务调度

类似的思想还有很多，例如数据库的连接池。线程池部分语言提供了原生支持，但`pthread`的基本接口并不支持，需要我们自己封装。如何封装本文提供一种解决思路。要解决两个基本的问题：

- 线程与任务的匹配
- 任务执行的调度，任务数据的同步

表面上线程池需要我们监控各个线程，将任务匹配到空闲线程中去，其实这样难度较大, 获取线程的状态、运行结果、管理同步等都比较麻烦。将视角放在任务身上问题就简单多了。任务就可以简单地抽象成一个带有返回值的函数，他需要某个线程去执行。我们不监控线程，而监控任务队列。只需要将一定的线程启动挂起，让线程监控任务队列中的任务，这样就起到了线程池调度任务的效果。

### 类设计
按照线程监控任务队列的思路，设计如下类：
```C++
#include <unistd.h>
#include <string>
#include <list>
#include <vector>
#include <pthread.h>

using std::list;
using std::vector;

class PthreadPool
{
public:
    // ThreadTask的实现先忽略，任务队列
    list<ThreadTask*> m_vTaskList;
    // To synchronize the shared var (usually as condition variable)
    pthread_mutex_t m_pthreadMutex;
    // synchronize condition signal
    pthread_cond_t m_pthreadCond;

    // 线程池销毁标志
    bool m_bDestroyAll;
    // 线程ID数组
    vector<pthread_t> m_vThreadID;

protected:
    void DestroyAllThread();
    void CreateAllThread(int threadNum);

public:
    // 静态成员函数 用作pthread_create的入参
    static void* ThreadFunc(void* threadData);

    PthreadPool(int threadNum);
    virtual ~PthreadPool();
    
    void AddTask(ThreadTask* t);
    int GetTaskSize();
};
```
线程池中线程需要最先启动挂起并开始监控任务队列，其核心就是`CreateAllThread`实现：

```C++
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
```

在这里不得不提一下`pthread_create`接口，他是典型的C接口风格函数。后两个参数要求是`(void*)(void*)`类型的函数指针和`void*` 类型的函数参数。这对C++的类成员函数就极不友好，因为有隐藏的`this`参数。普遍的解决思路是传入静态成员函数，为了使得函数能正常访问其他非静态成员变量，将`this`指针当做函数参数传入。我们将`ThreadFunc`作为线程池中所有线程的执行函数传入，他的实现就是执行任务队列的监控。

### 临界区同步
多线程的编程最重要的是临界区变量的同步，在线程池场景中显而易见从的临界区就是`任务队列`。多个线程需要同时读写的变量。同步的办法可以采用互斥锁+条件变量的方式。条件变量是信号量的升级，他可以实现多线程之间的次序通信，从而避免每个线层同时对临界区变量的轮询。首先轮询耗时，其次每次询问都要抢锁，比较浪费性能。条件变量可以协助通知，没接到临界区变化通知的线程一直挂起等待即可。

```C++

void* PthreadPool::ThreadFunc(void* threadData)
{
    if ( threadData == nullptr )
    {
        console_error("Thread data is null");
        return NULL;
    }
    // 类型转换
    PthreadPool* PoolData = (PthreadPool*)threadData;

    //loop for wait tasks
    while (true)
    {
        ThreadTask* pTask = nullptr;
        // 抢夺锁，用于保护临界区
        pthread_mutex_lock(&PoolData->m_pthreadMutex);
        while ( PoolData->m_vTaskList.empty() && !PoolData->m_bDestroyAll)
        {
            // 任务队列为空，挂起等待，同时释放mutex
            pthread_cond_wait(&PoolData->m_pthreadCond, &PoolData->m_pthreadMutex);
        }
        if ( !PoolData->m_vTaskList.empty() )
        {
            auto iter = PoolData->m_vTaskList.begin();
            pTask = *iter;
            PoolData->m_vTaskList.erase(iter);
        }
        // 操作完临界区，释放锁
        pthread_mutex_unlock(&PoolData->m_pthreadMutex);
        
        if ( pTask != nullptr )
        {
            pTask->Run();
            pTask->m_bFinished = true;
        }
        // 线程池销毁判断，收到销毁信息退出线程
        if ( PoolData->m_bDestroyAll && PoolData->m_vTaskList.empty() )
        {
            console_info("Thread exited");
            pthread_exit(NULL);
            break;
        }
    }
    return NULL;
}
```
唯一需要注意的就是`pthread_cond_wait(cond*, mutex*)`需要传入一个互斥锁，意思是挂起时释放锁给其他线程（不然添加任务的线程无法操作任务队列造成死锁）。

再来看下另外一个重要条件变量通知函数`AddTask`:

```C++
void PthreadPool::AddTask(ThreadTask* t)
{
    pthread_mutex_lock(&m_pthreadMutex);
    m_vTaskList.push_back(t);
    pthread_cond_signal(&m_pthreadCond);
    pthread_mutex_unlock(&m_pthreadMutex);
}
```
相同的步骤，获取锁保护临界区-->操作临界区-->发送条件变量信号-->释放锁。

### 任务体设计
上文中可以看出，任务必须要有的方法是`pTask->Run();` 其次也必须有是否结束的标识`m_bFinished`，其次为了能获取结果，任务也需要有自己的数据区。

```C++
class ThreadTask
{
private:
    // 友元，使线程池类能够使用task私有成员
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
    // 纯虚函数 必须实现
    virtual void Run() = 0;
    inline void SetData(void* data)
    {
        m_pData = data;
    }
    // 提供一个阻塞线程的方法
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
```
用户可以自定义任务继承此类。我们写一个简单的测试用例：
```C++
class TestTask : public ThreadTask
{
public:

    void Run()
    {
        sleep(1);
        done = true;
    }
    bool done;
};

// main.cpp测试：
pool = new PthreadPool(5);
clock_t start = clock();
TestTask* pTask = new TestTask[5];
for ( int ind = 0; ind < 5; ++ind) {
    pTask[ind].Init();
    pool->AddTask(&pTask[ind]);
}
// 等待任务队列全部完成
while ( true ) {
    bool all = true;
    for ( int ind = 0; ind < 5; ++ind)
    {
        if ( !pTask[ind].done )
            all = false;
    }
    if (all)
        break;
}
clock_t end = clock();
delete pool;
delete []pTask;
console_info("Time Consume: {}", (double)(end - start) / CLOCKS_PER_SEC);
```
测试线程池是否真正并行，运行四个一定会挂起的io就可以看出效果。

### 线程池销毁
最后，线程池的析构函数需要退出申请的线程资源，同时也要考虑正在运行的任务。
```C++
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
```
`pthread_cond_broadcast`强行唤醒所有等待信号线程，此时程序中需要注意临界区的判断，否则会引发未定义行为。`pthread_join`用于挨个等待进程，各个进程函数最终会由销毁标志触发`pthread_exit`退出线程。

### 改进空间
上文中使用`pthread`实现了一个基于任务调度思想的线程池，其缺点也有很多：

- 一开始就启动若干线程空挂不好，最高效的做法是用时申请
- 线程的执行状态只能依靠任务的状态变量得知，无法由线程池获取线程的状态。因为无法很好地设计阻塞代码，需要如测试用例一样轮询。
- 无法设置回调函数，全部逻辑需要在task内实现

上述缺点在现代C++标准库`<thread>`和`<future>`中有所解决，下一篇将继续研究cpp的线程池和异步场景使用。

## Reference

- [1] [一个pthreads教程](https://computing.llnl.gov/tutorials/pthreads/)
- [2] [在如今的Linux编程中，还有必要使用vfork()吗？](https://www.zhihu.com/question/304323673/answer/543471196)

- [3] [Linux下调用pthread库创建的线程是属于用户级线程还是内核级线程？](https://www.zhihu.com/question/35128513/answer/148038406)
- [4] [McCracken D. POSIX threads and the Linux kernel[C]//Ottawa Linux Symposium. 2002: 330.](https://scholar.google.com.hk/scholar?hl=zh-CN&as_sdt=0%2C5&q=POSIX+Threads+and+the+Linux+Kernel&btnG=)

- [5] [StackOverflow: Relationship between a kernel and a user thread](https://stackoverflow.com/questions/1178785/relationship-between-a-kernel-and-a-user-thread)
- [6] [Linux kernel scheduler](https://helix979.github.io/jkoo/post/os-scheduler/)

 