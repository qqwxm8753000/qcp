cmake_minimum_required(VERSION 3.16)
project(qcp)

set(CMAKE_CXX_STANDARD 11)

add_executable(qcp main.cpp)

target_link_libraries(qcp PRIVATE -static-libgcc -static-libstdc++)
