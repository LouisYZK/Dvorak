#ifndef HASH_MAP_H
#define HASH_MAP_H

#include <stdlib.h>
#include "MemBuffer.h"

using std::string;
using std::vector;

template <class VAL>
struct s_HashNode
{
    uint32_t keyNameIndex;
    VAL val;
    int nextNodeIndex;
};

template <class VAL>
class HashMap
{
    public:
        // Default ctor
        HashMap()
        {
            m_nTableSize = 0;
            m_nOffset = 0;
        };
        HashMap(uint32_t tableSize)
        {
            m_nTableSize = tableSize > 24 ? 24: tableSize;
            m_nOffset = 32 - m_nTableSize;
            if ( hash_.IsEmpty() )
            {
                hash_.Init(0, (1 << m_nTableSize));
            }
        }
        HashMap(const char* key_name_file, 
                const char* node_file,
                const char* map_file) :
                key_name_mb_(key_name_file), 
                node_mb_(node_file),
                hash_(map_file)
        {
            m_nTableSize = hash_.GetSize();
            m_nOffset = 24 - m_nTableSize;
        }

        ~HashMap() {};
        void Init(uint32_t tableSize);
        void Clear();
        void Insert(const string& key, const VAL& val);
        bool Find(const string& key, VAL& val);

        bool Keys(vector<string>& kyes);
        void Values(vector<VAL>& vals);
        uint32_t ValueSize();

        bool DumpBufferFile(const char* key_name_file, 
                const char* node_file,
                const char* map_file);
        
        MemBuffer<char> key_name_mb_;
        MemBuffer<s_HashNode<VAL>> node_mb_;
        MemBuffer<uint32_t> hash_;

    public:
        uint32_t m_nTableSize;
        uint32_t m_nOffset;
        uint32_t m_nSize;
        uint32_t Hash(const char* key) const
        {
            uint32_t hash = 0;
            while (*key)
            {
                hash = (*key++) + (hash << 6) + (hash << 16) - hash;
            }
            uint32_t sdb_hash = (hash & 0x7FFFFFFF);
            return ( sdb_hash << m_nOffset) >> m_nOffset;
        }
};


template <class VAL>
void HashMap<VAL>::Init(uint32_t tableSize)
{
    m_nTableSize = tableSize > 24 ? 24: tableSize;
    m_nOffset = 32 - m_nTableSize;
    if ( hash_.IsEmpty() )
    {
        hash_.Init(0, (1 << m_nTableSize));
    }
}

template <class VAL>
void HashMap<VAL>::Insert(const string& key, const VAL& val)
{
    auto nHash = Hash(key.c_str());
    auto nameIndex = key_name_mb_.AddObjects(key.c_str(), key.size());
    s_HashNode<VAL> new_node {nameIndex, val, -1};
    auto nodeIndex = node_mb_.AddObject(new_node);

    if ( hash_[nHash] == 0 )
    {
        m_nSize ++;
        hash_[nHash] = nodeIndex;
    } else
    {
        auto curNodeIndex = hash_[nHash];
        s_HashNode<VAL>* node = nullptr;
        node = node_mb_.GetObj(curNodeIndex); 
        while ( true ) 
        {
            if ( node->nextNodeIndex == -1)
            {
                node->nextNodeIndex = nodeIndex;
                m_nSize++;
                break;
            }
            node = node_mb_.GetObj(node->nextNodeIndex);
        }
    }
}

template <class VAL>
bool HashMap<VAL>::Find(const string& key, VAL& val)
{
    auto nHash = Hash(key.c_str());
    if ( hash_[nHash] == 0  && node_mb_.GetObj(0) == nullptr)
    {
        return false;
    } else
    {
        s_HashNode<VAL>* node = node_mb_.GetObj(hash_[nHash]);
        while ( true )
        {
            auto keyName = key_name_mb_.GetObj(node->keyNameIndex);
            if ( string{keyName} == key )
            {
                val = node->val;
                return true;
            }
            auto nextIndex = node->nextNodeIndex;
            if ( nextIndex == -1)
            {
                break;
            }
            node = node_mb_.GetObj(nextIndex);
        }
        if (node->nextNodeIndex == -1)
        {
            return false;
        } 
    }
    return true;
}

template <class VAL>
bool HashMap<VAL>::Keys(vector<string>& keys)
{
    for ( size_t ind = 0; ind < hash_.GetSize(); ++ind)
    {
        if ( hash_[ind] == 0)
            continue;
        auto node = node_mb_.GetObj(hash_[ind]);
        while ( true ) 
        {
            keys.push_back( string { key_name_mb_.GetObj(node->keyNameIndex) } );
            if ( node->nextNodeIndex == -1)
                break;
            node = node_mb_.GetObj(node->nextNodeIndex);
        }
    }
    return true;
}

template <class VAL>
void HashMap<VAL>::Values(vector<VAL>& keys)
{
    for ( size_t ind = 1; ind<= node_mb_.GetSize(); ++ind )
    {
        keys.push_back(node_mb_.GetObj(ind)->val);
    }
}

template <class VAL>
uint32_t HashMap<VAL>::ValueSize()
{
    return node_mb_.GetSize();
}

template <class VAL>
bool HashMap<VAL>::DumpBufferFile(const char* key_name_file, 
                const char* node_file,
                const char* map_file)
{
    if ( key_name_mb_.DumpBufferFile(key_name_file) && 
         node_mb_.DumpBufferFile(node_file) && 
         hash_.DumpBufferFile(map_file))
    {
        return true;
    }
    return false;
}
#endif