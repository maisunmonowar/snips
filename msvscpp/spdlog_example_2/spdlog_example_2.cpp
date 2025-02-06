
#include <iostream>

#include "spdlog/sinks/daily_file_sink.h"
#include <spdlog/sinks/stdout_color_sinks.h>

#include "manager.h"
int main()
{
    std::cout << "Hello World!\n";

    // Initialize logger
    auto fileLogger =
        std::make_shared<spdlog::sinks::daily_file_sink_mt>("test.log", 0, 0);
    auto consoleLogger =
        std::make_shared<spdlog::sinks::stdout_color_sink_mt>();

#ifdef _DEBUG
    consoleLogger->set_level(spdlog::level::debug);
    fileLogger->set_level(spdlog::level::debug);
#else
    consoleLogger->set_level(spdlog::level::info);
    fileLogger->set_level(spdlog::level::info);
#endif

    fileLogger->set_pattern("%Y-%m-%d %H:%M:%S.%e - %l - %v");

    // Create a logger with both sinks
    auto logger = std::make_shared<spdlog::logger>(
        "multi_sink", spdlog::sinks_init_list{consoleLogger, fileLogger});
#ifdef _DEBUG
    logger->set_level(spdlog::level::debug);
#else
    logger->set_level(spdlog::level::info);
#endif

    // Set the logger as the default logger
    spdlog::set_default_logger(logger);
    // logger initialized 

    spdlog::debug("message");

    int *intejar = new int;
    *intejar = 15;

    auto managerr = new manager();
    managerr->setName("Manager John");
    managerr->work(intejar);

}
