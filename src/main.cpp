#include "common/logger.hpp"
#include <torch/torch.h>
#include <iostream>
#include "serialize/PersonSerializer.h"

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
    log_info("nihao a!!!");

    torch::Tensor tensor = torch::eye(3);
    std::cout << tensor << std::endl;

    string inpath = "../data";
    string outputh = ".";
    string ver = "test";
    PersonSerializer ps(inpath, outputh, ver);
    ps.LoadStaticPerson();

    // s_Person_Test st {12100, "test", {13100, 12300} };
    // FILE * fp = fopen("person_byte", "wb");
    // fwrite(&st, sizeof(s_Person_Test), 1, fp);
    // fclose(fp);
    
    // FILE * fpr = fopen("person_byte", "rb");
    // s_Person_Test sr;
    // memset(&sr, 0, sizeof(s_Person_Test));
    // fread(&sr, sizeof(s_Person_Test), 1, fpr);
    // std::cout << &sr << std::endl;
    // std::cout << sr.nPersonID
    //            << sr.PersonName << std::endl;
    // std::cout << &sr.vPersonFriends << std::endl;
    // for ( auto& item : sr.vPersonFriends)
    //     std::cout << item << std::endl;
    // fclose(fpr);

    // std::cout << sizeof(Test) << std::endl;
}

