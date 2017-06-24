cmake_minimum_required(VERSION 3.7)
project(FinalJson)

set(CMAKE_C_STANDARD 99)

set(SOURCE_FILES main.c)
add_executable(FinalJson ${SOURCE_FILES})
target_link_libraries(FinalJson cjson curl pthread)
