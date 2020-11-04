#ifndef LOGGER_H
#define LOGGER_H

#include "spdlog/spdlog.h"
#include "spdlog/sinks/basic_file_sink.h"
#include "spdlog/sinks/stdout_color_sinks.h"

class Logger
{
    private:
        Logger();
        ~Logger();
        static Logger * instance;
    
    public:
        static Logger * getInstance();
        std::shared_ptr<spdlog::logger> logger;
        std::shared_ptr<spdlog::logger> console;
};
#endif