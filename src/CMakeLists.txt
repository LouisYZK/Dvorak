add_executable(main main.cpp)
link_directories(${PROJECT_BINARY_DIR}/lib)

# if(NOT TARGET spdlog)
#     find_package(spdlog REQUIRED)
# endif()

# target_link_libraries(main PRIVATE logger_static spdlog::spdlog)
target_link_libraries(main PRIVATE logger_static)

