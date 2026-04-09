cmake_minimum_required(VERSION 4.2)
project(circularBuffer C)

set(CMAKE_C_STANDARD 11)

include_directories(.)

add_executable(circularBuffer
        buffer_api.c
        buffer_api.h
        buffer_array.c
        buffer_array.h
        buffer_list.c
        buffer_list.h
        test.c)

find_package(Curses REQUIRED)
target_include_directories(circularBuffer PRIVATE ${CURSES_INCLUDE_DIRS})
target_link_libraries(circularBuffer PRIVATE ${CURSES_LIBRARIES})