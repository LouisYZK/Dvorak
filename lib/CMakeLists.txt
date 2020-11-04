
# if(NOT TARGET spdlog)
#     message(">>>>>> ??")
#     find_package(spdlog REQUIRED)
# endif()

set(LIBLOGGER_SRC logger.cpp)
# add_library(logger SHARED ${LIBLOGGER_SRC})
add_library(logger_static STATIC ${LIBLOGGER_SRC})
set_target_properties(logger_static PROPERTIES OUTPUT_NAME "logger")
# set_target_properties(logger PROPERTIES VERSION 1.1)
# target_link_libraries(logger PRIVATE spdlog::spdlog)
target_link_libraries(logger_static PRIVATE libspdlog.a)
