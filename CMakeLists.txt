cmake_minimum_required(VERSION 3.14)
project(software_library)

set(CMAKE_CXX_STANDARD 14)

add_executable(software_library main.cpp Date.cpp Date.h Book.cpp Book.h Library.cpp Library.h bptree_disk_based.cpp bptree_disk_based.h)