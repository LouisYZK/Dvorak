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
Logger::Logger()
{
    logger = spdlog::basic_logger_mt("basic_logger", "logs/info.txt");
    console = spdlog::stdout_color_mt("console");
}

Logger * Logger::instance = new Logger();
Logger * Logger::getInstance()
{
    return instance;
}

void log_output(std::string text)
{
    auto logger = Logger::getInstance()->console;
    logger->info(text);
}

#define log_info(text) log_output(text)

#endif
