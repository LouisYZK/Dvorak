#include "common/Logger.h"
#include <torch/torch.h>
#include <iostream>
#include "serialize/PersonSerializer.h"
#include "dataloader/PersonDataLoader.h"
#include "common/MemBuffer.h"
#include "common/HashMap.h"

using std::string;
struct s_Person_Test
{
    uint64_t nPersonID;
    string PersonName;
    vector<uint64_t> vPersonFriends;
};
struct Test
{
    uint64_t person_id;  // 8 bytes
    char     person_name[10]; // 10 * 1 = 10 bytes
    uint64_t person_friends[5]; // 5 * 8 = 40 bytes
};

template<class T>
void printe(T& ctn, const char* msg="")
{
    std::cout << std::endl;
    std::cout << msg << std::endl;

    typename T::const_iterator itr;
    for ( itr = ctn.begin();
            itr != ctn.end(); itr++ )
        std::cout << *itr << "\t";
    std::cout << std::endl;
}

int
main()
{
    // Logger * log = Logger::getInstance();
    auto log = Logger::getInstance()->logger;
    log->info("Welcome to Dvorak!");
    log->set_level(spdlog::level::debug);
    log->debug("This should disp...");
    
    auto console = Logger::getInstance()->console;
    console->info("Welcome to Dvorak!!");
    console_info("nihao a!!! {}\n", 123);

    torch::Tensor tensor = torch::eye(3);
    std::cout << tensor << std::endl;

    string inpath = "/Users/didi/xjtu/MY/data";
    string outputh = ".";
    string ver = "test";
    PersonSerializer ps(inpath, outputh, ver);
    auto ret = ps.Serialize();
    console_info("Serialize person {}", ret);

    HashMap<uint32_t> dct;
    dct.Init(4);
    dct.Insert("python", 111);
    dct.Insert("js", 222);
    
    vector<uint32_t> res;
    dct.Values(res);
    printe(res, "aaa");

    HashMap<s_Person> hm("key_name_file", "node_file", "hashmap");
    console_info("Person size: {}", hm.ValueSize());

    auto pd = data::PersonDataLoader::getInstance();
    auto rett = pd->LoadData(".");
    console_info("Person laoding ... {}, {}", rett, pd->GetPersonSize());


    for ( int ind = 1; ind < pd->GetPersonSize(); ind++)
    {
        s_Person_Info spi;
        pd->GetPersonByIndex(ind, spi);
        console_info("{} -> {} ", spi.PersonName, spi.nPersonID);
        for ( auto& item : spi.vPersonFriends)
        {
            console_info(" friends -> {}", item);
        }
    }
}


