#ifndef PERSON_SERIALIZER_H
#define PERSON_SERIALIZER_H

#include "BaseSerializer.h"
#include "rapidjson/document.h"
#include "rapidjson/rapidjson.h"
#include "rapidjson/stringbuffer.h"
#include <vector>

using std::vector;
using std::string;

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
            
};
#endif