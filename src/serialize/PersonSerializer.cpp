#include "PersonSerializer.h"
#include "../common/Logger.h"


void PersonSerializer::init() { }

void PersonSerializer::destroy() { }

int PersonSerializer::Load(void)
{
    init();
    if ( !LoadStaticPerson() )
    {
        destroy();
        printf("Load Static Person Failed\n");
        return BaseSerializer::FAILED;
    }
    destroy();
    return BaseSerializer::SUCCESS;
}
int PersonSerializer::Dump(void)
{
    bool ret = person_map_.DumpBufferFile("key_name_file", "node_file", "hashmap");
    if ( ret )
    {
        console_info("Dump {}, {}, {} ok!", "key_name_file", "node_file", "hashmap");
        return BaseSerializer::SUCCESS;
    }
    else
    {
        return BaseSerializer::FAILED;
    }
    
}

bool PersonSerializer::Serialize()
{
    if ( Load() && Dump() )
    {
        return BaseSerializer::SUCCESS;
    }
    else
    {
        return BaseSerializer::FAILED;
    }
    
}

bool PersonSerializer::LoadStaticPerson()
{    
    id_index_map_.Init(10);
    person_map_.Init(10);
    string filename = input_dir_ + "/person.json";
    FILE * fp = fopen(filename.c_str(), "r");
    if ( fp == nullptr)
    {
        log_error("Open file error!");
        return false;
    }
    
    char* buf = new char[1024 * 1024];
    vector<s_Person_Info> vec_person_info;
    vector<uint16_t> vIndex;
    while ( !feof(fp) )
    {
        memset(buf, 0, 1024 * 1024 * sizeof(char));
        fgets(buf, 1024 * 1024, fp);

        rapidjson::Document doc;
        doc.Parse(buf);
        if ( doc.HasParseError() )
        {
            log_error("Parse Json failed...\n");
        } else 
        {
            log_error("Paese success...\n");
        }
        
        rapidjson::Value& persons = doc["data"];
        assert(persons.IsArray());

        for ( size_t ind = 0; ind < persons.Size(); ++ind)
        {
            rapidjson::Value& item = persons[ind];
            string person_id = item["id"].GetString();
            string person_name = item["name"].GetString();

            auto res = name_mb_.AddObjects(person_id.c_str(), person_id.size());
            vIndex.push_back(res);
            id_index_map_.Insert(person_id, res);

            rapidjson::Value& friend_list = item["friends"];
            vector<string> vfriends;
            for ( size_t i = 0; i < friend_list.Size(); ++i)
            {
                vfriends.push_back(friend_list[i].GetString());
            }
            vec_person_info.push_back( { person_id, person_name, vfriends });
        }
    }
    vector<s_Person> vsPerson;
    auto ret = ConvertPerson(vec_person_info, vsPerson);
    if ( !ret )
        log_info(" Convert Failed.. ");

    for (auto i: vIndex)
    {
        // printf(">>>>>>>> %s | %s | %s\n", name_mb_.GetObj(i), &name_mb_[i], id_index_map_.key_name_mb_.GetObj(i));
        string ids {name_mb_.GetObj(i)};
        uint32_t index;
        auto ret = id_index_map_.Find(ids, index);
        s_Person sp;
        auto rett = person_map_.Find(ids, sp);
        console_info(" {} -> {} ", ids, sp.person_name_index);
        // if ( ret )
        // {
        //     printf("%s -> %d \n", ids.c_str(), index);
        // } else
        // {
        //     auto hashIndex = id_index_map_.Hash(ids.c_str());
        //     auto hashNode = id_index_map_.node_mb_.GetObj(id_index_map_.hash_[hashIndex]);
        //     int i = 0;
        //     while ( hashNode->nextNodeIndex != -1)
        //     {
        //         i ++;
        //         auto keyName = id_index_map_.key_name_mb_.GetObj(hashNode->keyNameIndex);
        //         hashNode = id_index_map_.node_mb_.GetObj(hashNode->nextNodeIndex);
        
        //     }
        // }
    }
    vector<string> vecs;
    id_index_map_.Keys(vecs);
    console_info(" size: {}, val {}, {}", vecs.size(), id_index_map_.hash_.GetSize(), id_index_map_.m_nSize);

}

bool PersonSerializer::ConvertPerson(
        const vector<s_Person_Info>& v_person_info,
        vector<s_Person>& vperson)
{
    for ( auto& pinfo: v_person_info )
    {
        s_Person sPer;
        auto nameIndex = name_mb_.AddObjects(pinfo.PersonName.c_str(), pinfo.PersonName.size());
        sPer.person_name_index = nameIndex;
        uint32_t idIndex;
        bool bFind = id_index_map_.Find(pinfo.nPersonID, idIndex);
        if ( !bFind )
        {
            log_error(" Not Found {}", pinfo.nPersonID);
            return false;
        }
        sPer.person_id_index = idIndex;

        for ( size_t ind = 0; ind < pinfo.vPersonFriends.size(); ++ind)
        {
            uint32_t index;
            string friendID = pinfo.vPersonFriends[ind];
            if ( !id_index_map_.Find(friendID, index))
            {
                log_error(" Not Found {} ", friendID);
                return false;
            }
            sPer.person_friends[ind] = index;
        }
        vperson.push_back(sPer);
        person_map_.Insert(pinfo.nPersonID, sPer);
    }
    return true;
}