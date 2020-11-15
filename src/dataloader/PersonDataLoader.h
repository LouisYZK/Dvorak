#ifndef PERSON_DATA_LOADER_H
#define PERSON_DATA_LOADER_H
#include "../serialize/struct.h"
#include <memory>
#include "../common/HashMap.h"

using std::string;
using std::vector;
using std::shared_ptr;

namespace data
{
    class PersonDataLoader
    {
        public:
            PersonDataLoader();
            ~PersonDataLoader();

            static PersonDataLoader * getInstance();

            bool LoadData(const string& dataPath);
            
            size_t GetPersonSize();
            
            bool GetNameByIndex( uint32_t index, string& name);
            bool GetNameByID( string& personID, string& name);

            bool GetPersonByIndex( uint32_t index, s_Person_Info& spi);
            bool GetPersonByID(string& personID, s_Person_Info& spi);

            bool GetFriendsByIndex( uint32_t index, vector<string> vfriends);
            bool GetFriendsByID( string& personID, vector<string> vfriends);  
            HashMap<s_Person>* hash_person_;

            
        private:
            static PersonDataLoader * instance;
            MemBuffer<char> name_mb_;
    };
};

#endif