cmake_minimum_required(VERSION 3.10)

add_definitions(-std=c++11)

project(Devorak CXX)

aux_source_directory(src SRC_LIST)

foreach(F ${SRC_LIST})
    message(${F})    
endforeach(F )


if(WIN32)
    message(">>>>> win os")
elseif(UNIX)
    message(">>>>> uninx os")
endif(WIN32)


add_subdirectory(lib)

add_subdirectory(src bin)

# set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
