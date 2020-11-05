add_executable(main main.cpp)
link_directories(${PROJECT_BINARY_DIR}/lib)

# if(NOT TARGET spdlog)
#     find_package(spdlog REQUIRED)
# endif()

set(Torch_DIR /Users/yangzhikai/libtorch/share/cmake/torch)
find_package(Torch REQUIRED)

set_property(TARGET main PROPERTY CXX_STANDARD 14)

# target_link_libraries(main PRIVATE logger_static spdlog::spdlog)
target_link_libraries(main PRIVATE logger_static "${TORCH_LIBRARIES}")

