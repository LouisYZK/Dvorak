
# if(NOT TARGET spdlog)
#     find_package(spdlog REQUIRED)
# endif()

set(LIBLOGGER_SRC logger.cpp)
add_library(logger_static STATIC ${LIBLOGGER_SRC})
set_target_properties(logger_static PROPERTIES OUTPUT_NAME "logger")
# target_link_libraries(logger PRIVATE spdlog::spdlog)
