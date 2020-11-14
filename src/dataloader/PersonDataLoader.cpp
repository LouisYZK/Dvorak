#include "PersonDataLoader.h"
#include "../common/Logger.h"

using namespace data;

PersonDataLoader::PersonDataLoader() {}

PersonDataLoader::~PersonDataLoader()
{
    if ( hash_person_ != nullptr )
    {
        delete hash_person_;
        hash_person_ = nullptr;
    }
}


PersonDataLoader* PersonDataLoader::instance = new PersonDataLoader(); 
PersonDataLoader* PersonDataLoader::getInstance()
{
    return instance;
}


bool PersonDataLoader::LoadData(const string& dataPath)
{
    string key_name_file = dataPath + "/key_name_file";
    string node_file = dataPath + "/node_file";
    string hashmap = dataPath + "/hashmap"; 
    string name_file = dataPath + "/name_file";
    hash_person_ = new HashMap<s_Person> { key_name_file.c_str(), node_file.c_str(), hashmap.c_str() };
    name_mb_.LoadBufferFile(name_file.c_str());
    if ( hash_person_->m_nTableSize != 0 )
    {
        console_info(" Load {} ok", key_name_file);
        console_info(" Load {} ok", node_file);
        console_info(" Load {} ok", hashmap);
        console_info(" Load {} ok", name_file);
        return true;
    }
    return false;
}

size_t PersonDataLoader::GetPersonSize()
{
    return hash_person_->node_mb_.GetSize();
}

bool PersonDataLoader::GetPersonByID( string& personID, s_Person_Info& spi)
{
    s_Person sp;
    if ( hash_person_->Find(personID, sp) )
    {
        auto idIndex = sp.person_id_index;
        spi.nPersonID = string { hash_person_->key_name_mb_.GetObj(idIndex) };
        auto nameIndex = sp.person_name_index;
        spi.PersonName = string { name_mb_.GetObj(nameIndex) };
        
        size_t ind;
        while ( true )
        {
            if ( sp.person_friends[ind] == 0 )
            {
                break;
            }
            spi.vPersonFriends.push_back( string {
                hash_person_->key_name_mb_.GetObj(
                    sp.person_friends[ind]
                )
            });
            ind++;
        }
        return true;
    }
    return false;
}


bool PersonDataLoader::GetPersonByIndex(uint32_t index, s_Person_Info& spi)
{
    if ( hash_person_ == nullptr )
    {
        return false;
    }
    s_HashNode<s_Person> node = *(hash_person_->node_mb_.GetObj(index));
    if ( node.keyNameIndex != 0 )
    {
        s_Person sp = node.val;
        
        auto idIndex = sp.person_id_index;
        spi.nPersonID = string { hash_person_->key_name_mb_.GetObj(idIndex) };
        auto nameIndex = sp.person_name_index;
        spi.PersonName = string { name_mb_.GetObj(nameIndex) };
        
        size_t ind = 0;
        while ( true )
        {
            if ( sp.person_friends[ind] == 0 )
            {
                break;
            }
            spi.vPersonFriends.push_back( string {
                hash_person_->key_name_mb_.GetObj(
                    sp.person_friends[ind]
                )
            });
            ind++;
        }
        return true;
    }
    return false;
}


