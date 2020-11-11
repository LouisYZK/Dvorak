#ifndef HASH_MAP_H
#define HASH_MAP_H

#include <stdlib.h>
#include "MemBuffer.h"

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
        void Insert(const string& key, const VAL& val);
        bool Find(const string& key, const VAL& val);
        
        MemBuffer<char> key_name_mb_;
        MemBuffer<s_HashNode> node_mb_;
        MemBuffer<uint32_t> hash_;
}


void HashMap<KEY, VAL>::Insert(const string& key, const VAL& val)
{
    auto nHash = Hash(key);
    auto nameIndex = key_name_mb_.AddObjects(key.c_str(), key.size());
    s_HashNode<VAL> node {nameIndex, val, -1};
    autp nodeIndex = node_mb_.AddObject(node);

    if ( !hash_[nHash] )
    {
        hash_[nHash] = nodeIndex;
    } else
    {
        auto curNodeIndex = hash_[nHash];
        auto* node = node_mb_.GetObj(curNodeIndex); 
        while (node->nextNodeIndex != -1)
        {
            node = node_mb_.GetObj(node->nextNodeIndex); 
        }
        node->nextNodexIndex = nodeIndex;
    }
}

bool HashMap<VAL>::Find(const string& key, const VAL& val)
{
    auto nHash = Hash(key);
    if ( !hash_[nHash] )
    {
        return false
    } else
    {
        auto keyIndex = key_name_mb_.GetObj(key);
        auto* node = hash_[nHash];
        while ( node->nextNodeIndex != -1 )
        {
            if (node->keyNameIndex == keyIndex )
            {
                break;
            }
            node = node_mb_.GetObj(node->nextNodeIndex);
        }
        if (node->nexNodeIndex == -1)
        {
            return false;
        } else {
            val = node->val;
            return true;
        }
    }
}
#endif