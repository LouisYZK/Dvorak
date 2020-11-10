#include "PersonSerializer.h"


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
        printf("error!\n");
        return false;
    }
    
    char* buf = new char[1024 * 1024];
    while ( !feof(fp) )
    {
        memset(buf, 0, 1024 * 1024 * sizeof(char));
        fgets(buf, 1024 * 1024, fp);

        rapidjson::Document doc;
        doc.Parse(buf);
        if ( doc.HasParseError() )
        {
            printf("Parse Json failed...\n");
        } else 
        {
            printf("Paese success...\n");
        }
        
        rapidjson::Value& persons = doc["data"];
        assert(persons.IsArray());
        vector<s_Person_Info> vec_person_info;
        
        for ( size_t ind = 0; ind < persons.Size(); ++ind)
        {
            rapidjson::Value& item = persons[ind];
            uint64_t person_id = item["id"].GetUint64();
            string person_name = item["name"].GetString();
            printf("%s, %lld...\n", person_name.c_str(), person_id);
        }

    }
}


// bool ConvertPerson(const s_Person_Info& person_info, s_Person& person)
// {
    
// }