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
    uint64_t id;  // 8 bytes
    uint64_t friends[5]; // 5 * 8 = 40 bytes
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

    // torch::Tensor tensor = torch::eye(3);
    // std::cout << tensor << std::endl;

    string inpath = "/Users/didi/xjtu/MY/Dvorak/data";
    string outputh = "dd";
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

    HashMap<Test> dict;
    Test t1; t1.id = 1; t1.friends[0] = 222;
    Test t2; t2.id = 10004; t2.friends[0] = 2223; t2.friends[1] = 1111;
    dict.Init(10);
    dict.Insert("test1", t1);
    dict.Insert("test2", t2);
    dict.DumpBufferFile("d/key", "d/node", "d/map");
    Test ttt; dict.Find("test1", ttt);
    console_info("hash1: {}", dict.hash_[dict.Hash("test2")]);
    console_info("Find {} {} {} {} {}", ttt.id, ttt.friends[0], ttt.friends[1], ttt.friends[2], ttt.friends[3]);

    HashMap<Test> dict2 {"d/key", "d/node", "d/map", 10};
    console_info("debug {} {} {}", *dict2.hash_.GetObj(1), dict2.node_mb_.GetObj(1)->nextNodeIndex, string{dict2.key_name_mb_.GetObj(1)});
    console_info("hash: {}, {}", dict2.hash_[dict.Hash("test2")], dict.Hash("test2"));
    const char* stest = "test2";
    Test tt1; dict2.Find(stest, tt1);
    // console_info("Find {} {} {}", tt1.id);

    const char* s = "hello";
    string ss = "hello";
    console_info(" {} A {} A {} -> {}", dict2.Hash(s), dict2.Hash(ss.c_str()), dict2.Hash("hello"), dict2.m_nOffset);

    auto pd = data::PersonDataLoader::getInstance();
    auto rett = pd->LoadData("dd");
    log_info("Person laoding ... {}, {}", rett, pd->GetPersonSize());
    
    for ( int ind = 1; ind < pd->GetPersonSize(); ind++)
    {
        s_Person_Info spi;
        bool res = pd->GetPersonByIndex(ind, spi);
        if (!res)
            continue;
        console_info("{} -> {} ", spi.PersonName, spi.nPersonID);
        for ( auto& item : spi.vPersonFriends)
        {
            console_info(" -------------- friends -> {}", item);
        }
    }
}


