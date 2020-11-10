#include "Logger.h"

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