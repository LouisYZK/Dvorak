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

}

bool PersonSerializer::LoadStaticPerson()
{
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
            printf("%s, %s..\n", person_name.c_str(), person_id.c_str());
            auto res1 = name_mb_.AddObjects(person_name.c_str(), person_name.size());
            auto res2 = name_mb_.AddObjects(person_id.c_str(), person_id.size());
            vIndex.push_back(res1);
            vIndex.push_back(res2);
        }
    }

    for (auto i: vIndex)
    {
        // console_info("{}", name_mb_.GetObj(i));
        printf(">>>>>>>> %s\n", name_mb_.GetObj(i));
    }
}


// bool ConvertPerson(const s_Person_Info& person_info, s_Person& person)
// {
    
// }