#include <string>
#include <vector>
#include <stdlib.h>
#include <stdio.h>

#pragma once

using std::string;
using std::vector;
#define FRIENDS_SIZE 6

// For use in memory
struct s_Person_Info
{
    string nPersonID;
    string PersonName;
    vector<string> vPersonFriends;
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