add_executable(main main.cpp)
link_directories(${PROJECT_BINARY_DIR}/lib)
# if(NOT TARGET logger)
# find_package(logger REQUIRED)

# if(NOT logger_FOUND)
#     message("Not found logger!")
# endif(NOT logger_FOUND)

# endif()
# target_link_libraries(main PRIVATE spdlog::spdlog logger)
target_link_libraries(main PRIVATE logger_static libspdlog.a)

