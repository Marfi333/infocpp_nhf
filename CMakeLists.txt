cmake_minimum_required(VERSION 3.17)
project(NHF4)

set(CMAKE_CXX_STANDARD 98)
add_compile_definitions(MEMTRACE)

add_executable(NHF4 main.cpp
        components.h components.cpp
        string5.h string5.cpp
        memtrace.h memtrace.cpp
        list.h
        file.cpp
        file.h
        controller.cpp controller.h jporta_test.cpp)

add_executable(JPORTA jporta_test.cpp
        components.h components.cpp
        string5.h string5.cpp
        memtrace.h memtrace.cpp
        list.h
        file.cpp
        file.h
        controller.cpp controller.h jporta_test.cpp)