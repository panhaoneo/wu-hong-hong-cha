set(CMAKE_EXECUTE_PROCESS_COMMAND_ECHO STDOUT)

cmake_minimum_required(VERSION 3.17)
project(wuhouhongcha LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 11)
message(status "wuhouhongcha Project Directory 1: ${PROJECT_SOURCE_DIR}")

add_definitions(-g -O2 -std=c++11 -Wno-deprecated -Wall -Werror ${CMAKE_CXX_FLAGS})

#include

include_directories(

)


#src
set(SRC_LIST 
    
    )

set(executable_output_path ./)

add_executable(wuhouhongcha ${SRC_LIST})
target_link_libraries(wuhouhongcha pthread)