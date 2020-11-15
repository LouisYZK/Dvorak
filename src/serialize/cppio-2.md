# [C++造轮子] 对象序列化实现方案 (二)

> [上文]()主要介绍了对象序列化编码的背景和C++仅使用原生二进制接口实现的思路，本篇将实现到工程落实，主要内容有：

- 编解码类设计-使用索引
- 基础内存管理工具设计
- 哈希表设计

- [原文地址](http://zhikai.pro/post/97)
- [本文代码地址](https://github.com/LouisYZK/Dvorak/tree/master/src/serialize)


## 基于索引的序列化设计

### Recap:类型设计
上文中针对C++结构体对象中的动态数组和字符串，提出了采用 “索引” + “紧凑存储” 的方式处理，对象中只保留动态成员的索引，从而将长度不定的动态对象转换为只包含基本类型的结构体。

```C++
// 不定长动态对象
struct s_Person_Test
{
    string nPersonID;
    string PersonName;
    vector<uint64_t> vPersonFriends;
};
// 内存对齐的、只包含基本类型的定长对象
struct s_Person
{
    uint32_t person_name_index;
    uint32_t person_id_index;
    uint32_t person_friends[FRIENDS_SIZE];
};
```

### 内存设计
内存设计是指序列化对象在内存中的存储形式，例如简单的数组（顺序排列），链表等，这对后序设计`dump`与`load`相关功能紧密联系。

`s_Person`使用索引与两个字符串成员关联，上文我们初步提出了下图的紧凑方案：

![](https://pic.downk.cc/item/5fa938871cd1bbb86b857396.png)

紧凑方案的索引寻址特点：

- 对于定长对象，利用反射机制（动态语言）或存储记录下类型长度，即可直接按照下标寻址
- 对于不定长度对象，可以利用“起始位置+终止位置”两个索引配合寻址

这样寻址的复杂度都是常书级，且空间利用足够节约。但对于`s_Person`的字符串来说，C++有更好的方案，“终止位置”可以由字符`\0`代替。在C系列语言的字符串设计中，基本是按照终止符的原则。这样只需记录起始位置即可，但要注意序列化时存储终止符。

## Memory Buffer工具的设计
上图中的一切都是在内存中的形态，无论是现行连续存储的定长结构体和字符串，还是个人维护的哈希表对象，在任何语言中我们都要使用数据结构在内存中维护。我们先讨论定长的结构体和字符串（已经理解为字符数组）。

### 设计一个知晓内存分布的容器
C++在内存中维护上文中的对象很容器，STL提供了大量的数据结构供选择，`std::vector<s_Person>`，`unordered_map<string, int>`等都可以再内存中实现数据存取。但问题是他们并无`dump`方法和`load`方法。

> 所谓dump和load就是序列化和反序列化在各语言经常用的方法名，一般dump是指内存对象到二进制，load是二进制到内存，一般这些方法还可以指定文件或是字符串。如果指定文件，那么就是dump into file, 二进制落盘存储， load就是load from file, 再读二进制文件到内存对象。如果是string, 就是指编码方式由二进制转为字符串。

`dump`方法和`load`方法都涉及到IO读写，前提是设计者必须清楚内存布局，这样才能调用基本的读写函数。STL的对象显然不合适，除非你很清楚Allocator背后的逻辑，能够无误地写出编解码容器的方法。

既然STL的容器不是“知根知底”，我们只能设计一个简易的内存容器帮助我们管理定长对象。他主要的功能有：

- 在内存中存储定长对象（只包含基本类型的结构体、字符等），要求有泛化能力
- 能根据下标索引出对象（字符数组采用休止符）
- 容器大小动态扩容 （这一点与STL容器要求一致，一次性申请的做法应被抛弃）
- 具备`dump`和`load`到文件的方法

这样一个容器类可以命名为`MemBuffer`,buffer就是内存与磁盘io交互之间的缓冲区，本意是避免过多的读盘操作。根据我们的需求，这个类的基本声明如下：

```C++
//设计两级数组内存布局，适应动态扩容
#define FIRST_LEVEL_BIT_ 8 // 一级数组长度 1 << 8
#define SECOND_LEVEL_BIT_ 8 // 二级数组长度 1 << 8

template <class T>
class MemBuffer
{
public:
    //构造函数，可以从文件中构造或构造空容器
    MemBuffer(const char* fileName = NULL,
             uint16_t firstLevel = FIRST_LEVEL_BIT_,
             uint16_t secondLevel = SECOND_LEVEL_BIT_);

    MemBuffer(uint16_t firstLevel, uint16_t secondLevel);

    ~MemBuffer();

    //先不设计各类拷贝构造
    MemBuffer(const MemBuffer& rhs) = delete;

    //添加成员，返回下标
    uint32_t AddObject(const T& obj);
    uint32_t AddObjects(const T* objs, int nLength);

    //根据下标获取成员
    T* GetObj(uint32_t nIndex);
    const T* GetObj(uint32_t nIndex) const;

    bool IsEmpty() const;

    void Init(const T& val, uint32_t size);
    void Reserve(uint32_t size);
    void Clear();

    //从文件中dump与load
    bool LoadBufferFile(const char* fileName);
    bool DumpBufferFile(const char* fileName);

    T& operator[](uint32_t nIndex);
    const T& operator[](uint32_t nIndex) const;
    uint32_t GetSize();

private:
    uint16_t FIRST_LEVEL_BIT;
    uint16_t SECOND_LEVEL_BIT;

    //记录当前成员的游标位置
    unsigned short m_nCurFirstIndex;
    unsigned short m_nSecondIndex;

    //二级数组指针
    T** m_ppFirstBuffer;
};
```
可以看出来很多方法名都是模仿STL的规律来的，只是内存管理自己设计，知晓内存布局可以清楚地设计相关的IO操作。

> 二级数组内存管理：（逻辑上）每次开辟一个一位数组（一行），扩容时再开辟一行。

我们看下关键的`Add`与`Get`实现：

```C++
template <class T>
uint32_t MemBuffer<T>::AddObject(const T& obj)
{
    //是否需要开辟新内存
    if ( !m_ppFirstBuffer[m_nCurFirstIndex] )
    {
        m_ppFirstBuffer[m_nCurFirstIndex] = new T[( 1<< SECOND_LEVEL_BIT )];
        memset(m_ppFirstBuffer[m_nCurFirstIndex], 0, 
                    sizeof(T) * (1 << SECOND_LEVEL_BIT));
    }

    //计算当前下标
    uint32_t index = ( m_nCurFirstIndex << FIRST_LEVEL_BIT ) + m_nSecondIndex;
    //写入对象
    m_ppFirstBuffer[m_nCurFirstIndex][m_nSecondIndex] = obj;
    m_nSecondIndex++;
    if ( m_nSecondIndex == (uint16_t)( 1 << SECOND_LEVEL_BIT ) )
    {
        m_nCurFirstIndex++;
        m_nSecondIndex = 0;
    }
    return index;   
}
```
对于字符数组和其他数组，设计的`AddObjects`方法多一步写入终止符符号：

```C++
template <class T>
uint32_t MemBuffer<T>::AddObjects(const T* objs, int nLength)
{
    /*
        ....
    */
    uint32_t index = ( m_nCurFirstIndex << FIRST_LEVEL_BIT ) + m_nSecondIndex;
    memcpy(m_ppFirstBuffer[m_nCurFirstIndex] + m_nSecondIndex,
                objs, sizeof(T) * nLength);
    m_nSecondIndex += nLength;

    // Tricks: Use \0 in C as natural Separator so that we can 
    // find item only using its start position without its length.
    memset(m_ppFirstBuffer[m_nCurFirstIndex] + m_nSecondIndex++,
                0, sizeof(T));
}
```
虽然逻辑上采用了二级数组存储，但实际中的内存地址只有一维连续的，返回的下标也是实际位置，寻址时需要做运算转换：

```C++
template <class T>
T* MemBuffer<T>::GetObj(uint32_t nIndex)
{
    //计算一维下标
    uint16_t firstIndex = ( nIndex >> FIRST_LEVEL_BIT );
    //计算二维下标
    uint16_t secondIndex = nIndex - (firstIndex << FIRST_LEVEL_BIT);
    return m_ppFirstBuffer[firstIndex] + secondIndex;
}
```
目前的功能基本具备了泛化动态容器的能力，基于此再设计io方法。
### 设计dump和load
基本存取中使用了大量的`memeset`和`memcpy`的基础接口，文件读写也同样只使用`fwrite`和`fread`,因为此时我们对内存的存储策略已经很清楚：

```C++
template <class T>
bool MemBuffer<T>::DumpBufferFile(const char * fileName)
{
    FILE* fp = fopen(fileName, "wb");
    
    uint32_t nCurIndex = ( m_nCurFirstIndex << FIRST_LEVEL_BIT ) + m_nSecondIndex;
    uint32_t nSize = sizeof(T);
    
    // Firt two 4B write the content size;
    fwrite(&nCurIndex, sizeof(uint32_t), 1, fp);
    fwrite(&nSize, sizeof(uint32_t), 1, fp);

    uint16_t realFirstIndex = m_nSecondIndex ? m_nCurFirstIndex : m_nCurFirstIndex - 1;
    
    for (int i = 0; i <= realFirstIndex; ++i)
    {
        fwrite(m_ppFirstBuffer[i], ((1 << SECOND_LEVEL_BIT ) * sizeof(T)), 1, fp);
    }
    fclose(fp);
    return true;
}

template <class T>
bool MemBuffer<T>::LoadBufferFile(const char * fileName)
{
    FILE* fp = fopen(fileName, "rb");
    uint32_t nIndex;
    uint32_t nSize;
    fread(&nIndex, sizeof(uint32_t), 1, fp);
    fread(&nSize, sizeof(uint32_t), 1, fp);

    m_nCurFirstIndex = (nIndex >> SECOND_LEVEL_BIT);
    m_nSecondIndex  = nIndex - (m_nCurFirstIndex << SECOND_LEVEL_BIT);

    uint32_t realFirtstIndex = m_nSecondIndex ? 
                m_nCurFirstIndex : m_nCurFirstIndex - 1;

    for ( int i = 0; i <= realFirtstIndex; ++i)
    {
        if ( !m_ppFirstBuffer[i] )
        {
            m_ppFirstBuffer[i] = new T[1 << SECOND_LEVEL_BIT];
            memset(m_ppFirstBuffer[i], 0, sizeof(T) * SECOND_LEVEL_BIT);
            fread(m_ppFirstBuffer[i], (1 << SECOND_LEVEL_BIT) * sizeof(T), 1, fp);
        }
    }
    fclose(fp);
    return true;
}
```
使用自主设计的`MemBuffer`类就就可以尝试将`s_Person`数组和 字符数组存储、加载和管理了。现在未解决就是哈希表了。

## 哈希表设计

## 后序

## 总结

