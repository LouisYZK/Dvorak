
# if(NOT TARGET spdlog)
#     find_package(spdlog REQUIRED)
# endif()
set(LIBLOGGER_SRC Logger.cpp)
add_library(logger_static SHARED ${LIBLOGGER_SRC})

set(LIBTHREAD PthreadPool.cpp)
add_library(pool SHARED ${LIBTHREAD})
target_link_libraries(pool logger_static)

# set(MEM_SRC MemBuffer.cpp)
# add_library(membuffer STATIC ${MEM_SRC})
# set_target_properties(logger_static PROPERTIES OUTPUT_NAME "logger")
# target_link_libraries(logger PRIVATE)
