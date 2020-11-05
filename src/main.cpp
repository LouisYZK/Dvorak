#include "../lib/logger.h"
#include <torch/torch.h>
#include <iostream>
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

    torch::Tensor tensor = torch::eye(3);
    std::cout << tensor << std::endl;
}

