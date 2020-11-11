#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "Logger.h"

#ifndef MEMBUFFER_H
#define MEMBUFFER_H
 
#define FIRST_LEVEL_BIT_ 8
#define SECOND_LEVEL_BIT_ 8
enum AccessMode
{
    MEMORY = 0,
    DIS = 1
};

template <class T>
class MemBuffer
{
    public:
        MemBuffer(const char* fileName = NULL,
                  uint16_t firstLevel = FIRST_LEVEL_BIT_,
                  uint16_t secondLevel = SECOND_LEVEL_BIT_,
                  int mode = MEMORY);

        MemBuffer(uint16_t firstLevel, uint16_t secondLevel);

        ~MemBuffer();

        MemBuffer(const MemBuffer& rhs) = delete;

        uint32_t AddObject(const T& obj);
        uint32_t AddObjects(const T* objs, int nLength);

        T* GetObj(uint32_t nIndex);
        const T* GetObj(uint32_t nIndex) const;

        bool IsEmpty() const;

        void Init(const T& val, uint32_t size);

        // void Reserve(uint32_t size);

        void Clear();

        bool LoadBufferFile(const char* fileName);
        bool DumpBufferFile(const char* fileName);

        // T& operator[](uint32_t nIndex);
        // const T& operator[](uint32_t nIndex) const;

    public:
        uint16_t FIRST_LEVEL_BIT;
        uint16_t SECOND_LEVEL_BIT;

        unsigned short m_nCurFirstIndex;
        unsigned short m_nSecondIndex;

        T** m_ppFirstBuffer;
        char * m_FileName;
        int m_AccessMode;
        FILE * m_ptrFile;
        bool bLoadFileSuccess;

};

template<class T>
MemBuffer<T>::MemBuffer(const char* fileName, uint16_t firstLevel,
                        uint16_t secondLevel, int mode) :
              FIRST_LEVEL_BIT(firstLevel), SECOND_LEVEL_BIT(secondLevel)
{
    bLoadFileSuccess = true;
    m_nCurFirstIndex = 0;
    m_nSecondIndex = 1;

    m_ppFirstBuffer = new T* [1 << FIRST_LEVEL_BIT];
    memset(m_ppFirstBuffer, 0, sizeof(T*) * (1 << FIRST_LEVEL_BIT));

    m_FileName = nullptr;
    if ( fileName )
    {
        m_FileName = new char[strlen(fileName) + 1];
        if (m_FileName)
        {
            memset(m_FileName, 0, strlen( m_FileName) + 1);
            strcpy(m_FileName, fileName);
            
            if (mode == MEMORY)
            {
                LoadBufferFile(m_FileName);
            }
        }
    }

    m_AccessMode = mode;
    m_ptrFile = nullptr;
}

template<class T>
MemBuffer<T>::MemBuffer(uint16_t firstLevel, uint16_t secondLevel) :
            FIRST_LEVEL_BIT(firstLevel), SECOND_LEVEL_BIT(secondLevel)
{
    bLoadFileSuccess = true;
    m_FileName = nullptr;
    m_nCurFirstIndex = 0;
    m_nSecondIndex = 1;
    m_ppFirstBuffer = new T* [sizeof(T*) * (1 << FIRST_LEVEL_BIT)];
    m_AccessMode = 0;
    m_ptrFile = nullptr;
}

template <class T>
MemBuffer<T>::~MemBuffer()
{
    if ( m_FileName )
    {
        delete[] m_FileName;
        m_FileName = nullptr;
    }
    
    if ( m_ppFirstBuffer )
    {
        for (int i = 0; i < m_nCurFirstIndex; ++i)
        {
            if (m_ppFirstBuffer[i])
                delete[] m_ppFirstBuffer[i];
            m_ppFirstBuffer[i] = nullptr;
        }
        delete[] m_ppFirstBuffer;
        m_ppFirstBuffer = nullptr;
    }

    if ( m_ptrFile )
    {
        fclose(m_ptrFile);
        m_ptrFile = nullptr;
    }
}

template <class T>
bool MemBuffer<T>::IsEmpty() const
{
    return ( m_nCurFirstIndex == 0 
                && m_nSecondIndex == 1);
}

template <class T>
uint32_t MemBuffer<T>::AddObject(const T& obj)
{
    if ( !m_ppFirstBuffer[m_nCurFirstIndex] )
    {
        m_ppFirstBuffer[m_nCurFirstIndex] = new T[( 1<< SECOND_LEVEL_BIT )];
        memset(m_ppFirstBuffer[m_nCurFirstIndex], 0, 
                    sizeof(T) * (1 << SECOND_LEVEL_BIT));
    }

    uint32_t index = ( m_nCurFirstIndex >> FIRST_LEVEL_BIT ) + m_nSecondIndex;
    m_ppFirstBuffer[m_nCurFirstIndex][m_nSecondIndex] = obj;
    m_nSecondIndex++;

    if ( m_nSecondIndex == (uint16_t)( 1 << SECOND_LEVEL_BIT ) )
    {
        m_nCurFirstIndex++;
        m_nSecondIndex = 0;
    }
    return index;   
}

template <class T>
uint32_t MemBuffer<T>::AddObjects(const T* objs, int nLength)
{
    if ( !m_ppFirstBuffer[m_nCurFirstIndex] )
    {
        m_ppFirstBuffer[m_nCurFirstIndex] = new T[( 1<< SECOND_LEVEL_BIT )];
        memset(m_ppFirstBuffer[m_nCurFirstIndex], 0,
                    sizeof(T) * (1 << SECOND_LEVEL_BIT));
    }

    if ( m_nSecondIndex + nLength + 1 > ( 1 << SECOND_LEVEL_BIT) )
    {
        m_nCurFirstIndex++;
        m_nSecondIndex = 0;
        m_ppFirstBuffer[m_nCurFirstIndex] = new T[( 1<< SECOND_LEVEL_BIT )];
        memset(m_ppFirstBuffer[m_nCurFirstIndex], 0, 
                        sizeof(T) * (1 << SECOND_LEVEL_BIT));
    }

    uint32_t index = ( m_nCurFirstIndex << FIRST_LEVEL_BIT ) + m_nSecondIndex;
    memcpy(m_ppFirstBuffer[m_nCurFirstIndex] + m_nSecondIndex,
                objs, sizeof(T) * nLength);
    m_nSecondIndex += nLength;

    // Tricks: Use \0 in C as natural Separator so that we can 
    // find item only using its start position without its length.
    memset(m_ppFirstBuffer[m_nCurFirstIndex] + m_nSecondIndex++,
                0, sizeof(T));

    if ( m_nSecondIndex == (uint16_t)( 1 << SECOND_LEVEL_BIT ) )
    {
        m_nCurFirstIndex++;
        m_nSecondIndex = 0;
    }
    return index;   
}

template <class T>
T* MemBuffer<T>::GetObj(uint32_t nIndex)
{
    uint16_t firstIndex = ( nIndex >> FIRST_LEVEL_BIT );
    uint16_t secondIndex = nIndex - (firstIndex << FIRST_LEVEL_BIT);
    // console_info("{}", firstIndex, secondIndex);
    return m_ppFirstBuffer[firstIndex] + secondIndex;
}
        
template<class T>        
const T* MemBuffer<T>::GetObj(uint32_t nIndex) const
{
    uint16_t firstIndex = ( nIndex >> FIRST_LEVEL_BIT );
    uint16_t secondIndex = nIndex - (firstIndex << FIRST_LEVEL_BIT);
    return m_ppFirstBuffer[firstIndex] + secondIndex;
}

template <class T>
bool MemBuffer<T>::DumpBufferFile(const char * fileName)
{
    FILE* fp = fopen(fileName, "wb");
    if ( fp == nullptr )
    {
        log_error("Open File Error! {}", fileName);
        fclose(fp);
        return false;
    }
    
    uint32_t nCurIndex = ( m_nCurFirstIndex << FIRST_LEVEL_BIT ) + m_nSecondIndex;
    uint32_t nSize = sizeof(T);
    
    // Firt two 4B write the content size;
    fwrite(&nCurIndex, sizeof(uint32_t), 1, fp);
    fwrite(&nSize, sizeof(uint32_t), 1, fp);

    uint16_t realFirstIndex = m_nSecondIndex ? m_nCurFirstIndex : m_nCurFirstIndex - 1;
    
    for (int i = 0; i <= realFirstIndex; ++i)
    {
        if ( !m_ppFirstBuffer[i] )
            break;
        fwrite(m_ppFirstBuffer[i], (1 << SECOND_LEVEL_BIT ), 1, fp);
    }
    fclose(fp);

    return true;
}

template <class T>
bool MemBuffer<T>::LoadBufferFile(const char * fileName)
{
    FILE* fp = fopen(fileName, "rb");
    if ( fp == nullptr )
    {
        log_error("Open file error! {}", fileName);
        return false;
    }

    uint32_t nIndex;
    uint32_t nSize;
    fread(&nIndex, sizeof(uint32_t), 1, fp);
    fread(&nSize, sizeof(uint32_t), 1, fp);

    if ( nIndex == 0)
    {
        log_error("Null File!");
        return false;
    }

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
            
            int retSize = fread(m_ppFirstBuffer[i], (1 << SECOND_LEVEL_BIT), 1, fp);
            if ( !retSize )
            {
                log_error("Read Failed...");
                fclose(fp);
                return false;
            }
        }
    }
    return true;
}

template <class T>
void MemBuffer<T>::Clear()
{
    if ( m_ppFirstBuffer )
    {
        for ( int i = 0; i <= m_nCurFirstIndex; ++i)
        {
            if ( m_ppFirstBuffer[i] )
            {
                delete[] m_ppFirstBuffer[i];
                m_ppFirstBuffer[i] = nullptr;
            }
        }
    }
    memset(m_ppFirstBuffer, 0, sizeof(T*) * (1 << FIRST_LEVEL_BIT));
    m_nCurFirstIndex = 0;
    m_nSecondIndex = 1;
}

#endif