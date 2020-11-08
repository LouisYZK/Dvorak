#ifndef BASE_SERIALIZER_H
#define BASE_SERIALIZER_H

#include <string>
#include <memory.h>
#include <stdio.h>

class BaseSerializer
{
    public:
        const static int SUCCESS = 0;
        const static int FAILED = 1;
        virtual int Load() = 0;  // For load data source to some format in memeory
        virtual int Dump() = 0;  // Dump the obj in memeory into Byte Serials.

        BaseSerializer() {}
        BaseSerializer(std::string input_dir, 
                       std::string output_dir,
                       std::string version) :
                        input_dir_(input_dir), 
                        output_dir_(output_dir),
                        version_(version) {}

        virtual ~BaseSerializer() {}

    protected:
        std::string input_dir_;
        std::string output_dir_;
        std::string version_;      
};

#endif