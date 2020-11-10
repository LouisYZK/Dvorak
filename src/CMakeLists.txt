if(NOT TARGET spdlog)
    find_package(spdlog REQUIRED)
endif()

include_directories(/Users/didi/rapidjson/include)
add_executable(main main.cpp)
link_directories(${PROJECT_BINARY_DIR}/lib)

# set(Torch_DIR /Users/yangzhikai/libtorch/share/cmake/torch)
set(Torch_DIR /Users/didi/Reading-Group/libtorch/share/cmake/torch)
find_package(Torch REQUIRED)

add_subdirectory(serialize)
add_subdirectory(common)
set_property(TARGET main PROPERTY CXX_STANDARD 14)

# target_link_libraries(main PRIVATE logger_static spdlog::spdlog)
target_link_libraries(main PRIVATE "${TORCH_LIBRARIES}" logger_static serialize spdlog::spdlog)

