# [C++造轮子] 实现一个线程池

> 本篇是C++轮子系列的第二个主题，之前第一个主题为对象序列化和数据加载相关。C++工程很多基础模块需要自己封装，常见的线程池是其中一项。线程池在IO密集、并发任务、异步任务、后台监控任务场景中都发挥作用。而`线程`话题无论是操作系统和高级编程语言上都是一个复杂的话题。本文的讨论是基于Posix标准讨论的。

> 题外话，作为一名pythonista, 每当听到身边同行对python多线程的吐槽都十分无语，他们大多不清楚GIL出现的原因、适合什么场景、现在PEP给出了哪些解决方案、是否其他的python实现规避了GIL等等问题。只是单纯地知道python的`伪多线程`。使用cpython至今，仍然没找到需要语言层面出面解决计算密集任务的场景，因各类计算框架没有一个只用python原生实现。而web中单纯的io场景无论是协程、多线程、第三方事件循环都解决的很好。

本文的主要内容有：

- Linux线程与pthread库
- 基于pthread库的线程池实现
- 基于现代C++的线程池实现


## Linux线程与内核支持

## Posix标准的实现

## 基于pthread(NTPL)实现线程池基本功能

## 现代C++的线程池


[一个pthreads教程](https://computing.llnl.gov/tutorials/pthreads/)

创建时最终都会调用do_fork函数，不同之处是传入的参数不同（clone_flags），最终结果就是进程有独立的地址空间和栈，而用户线程可以自己指定用户栈，地址空间和父进程共享，内核线程则只有和内核共享的同一个栈，同一个地址空间。当然不管是进程还是线程，do_fork最终会创建一个task_struct结构。

getconf GNU_LIBPTHREAD_VERSION

kernel scheduling entities 

[StackOverflow: Relationship between a kernel and a user thread](https://stackoverflow.com/questions/1178785/relationship-between-a-kernel-and-a-user-thread)
[Linux kernel scheduler](https://helix979.github.io/jkoo/post/os-scheduler/)

作者：r00tk1t
链接：https://www.zhihu.com/question/304323673/answer/543471196
来源：知乎
著作权归作者所有。商业转载请联系作者获得授权，非商业转载请注明出处。

另外关于Linux内核的进程和线程，实际上由于历史原因，Linux也遵守了真香定律。以pthread的两种实现LinuxThread（Linux2.4以前）和NPTL（2.5以后）为例，早期内核是没有TGID的，只有PID，所以内核根本就没有线程的概念，不管有没有CLONE_VM，在内核眼里都是进程，都是调度器调度的单元，而这就给LinuxThread的实现带来了难题——LinuxThread采用1:1模型，即每个线程都是LWP对应一个内核线程（这个线程的概念是从我们的视角出发的），在内核不支持的情况下如何实现线程的一套同步互斥呢？LinuxThread使用信号来模拟，显然效率不高。而NPTL时代，内核引入TGID，此时依然是1:1模型，但不是简单的LWP了，TGID把这些线程联系了起来，那如何告知内核呢？答案就是NPTL创建线程时传递一个CLONE_THREAD 标志，所以用的是clone。内核把TGID填写为调用者的PID，PID填写新线程号（原本的进程号）。有了这一层联系，线程的同步互斥就可以依赖于用户空间的锁（因为大家共享同一片地址空间），省去了此前内核态切来切去的麻烦，效率自然高了，也就是所谓的futex。


[Linux下调用pthread库创建的线程是属于用户级线程还是内核级线程？](https://www.zhihu.com/question/35128513/answer/148038406)
[McCracken D. POSIX threads and the Linux kernel[C]//Ottawa Linux Symposium. 2002: 330.](https://scholar.google.com.hk/scholar?hl=zh-CN&as_sdt=0%2C5&q=POSIX+Threads+and+the+Linux+Kernel&btnG=)