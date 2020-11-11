#ifndef PERSON_SERIALIZER_H
#define PERSON_SERIALIZER_H

#include "BaseSerializer.h"
#include "rapidjson/document.h"
#include "rapidjson/rapidjson.h"
#include "rapidjson/stringbuffer.h"
#include "../common/MemBuffer.h"
#include <vector>
#include <unordered_map>

using std::vector;
using std::string;
using std::unordered_map;

#define FRIENDS_SIZE 6
// For use in memory
struct s_Person_Info
{
    uint64_t nPersonID;
    string PersonName;
    vector<uint64_t> vPersonFriends;
};

// For bytes 
struct s_Person
{
    uint32_t person_name_index;
    uint32_t person_id_index;
    uint32_t person_friends[FRIENDS_SIZE];
    
    s_Person()
    {
        memset(this, 0, sizeof(s_Person));
    }
};

// id -> index
struct s_PersonHashNode
{
    uint32_t nIndex;  // pos in vector of s_Person;
    uint32_t nKeyNameIndex; // key(id)'s pos in name_buffer_;
    uint32_t nNextNodeIndex; // next node's index in nodelist_membuffer_;
};


class PersonSerializer : BaseSerializer
{
    public:
        PersonSerializer(string input_path, string output_path,
                         string version) :
                        BaseSerializer(input_path, output_path, version)
                        { }
        bool LoadStaticPerson();
    
    private:
        void init();
        void destroy();
        int Load(void);
        int Dump(void);

        vector<s_Person> vec_persons_;
        uint32_t person_size_;
        unordered_map<uint32_t, s_Person> person_map_;

        MemBuffer<s_Person> person_mb_;
        MemBuffer<char> name_mb_;     
};
#endif