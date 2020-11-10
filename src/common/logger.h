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
        
        enum LogLevel {debug, info, notice, error};
};

#define CONSOLE_OUTPUT(level, format, ...)          \
do {                                                \
    auto console = Logger::getInstance()->console;  \
    switch (level)                                  \
    {                                               \
        case Logger::info:                          \
            console->info(format, ##__VA_ARGS__); break;\
        case Logger::error:                         \
            console->error(format,  ##__VA_ARGS__); break;\
        default:                                    \
            break;                                  \
    }                                               \
} while (false)

#define console_info(format, args...) CONSOLE_OUTPUT(Logger::info, format, ##args)
#define console_error(format, args...) CONSOLE_OUTPUT(Logger::error, format, ##args)

#endif
