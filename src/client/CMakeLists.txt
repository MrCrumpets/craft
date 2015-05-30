cmake_minimum_required(VERSION 3.2)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

include_directories(/home/nathan/Downloads/asio-1.10.6/include)
find_package(Threads)

set(SOURCE_FILES main.cpp)

add_executable(client ${SOURCE_FILES})
target_link_libraries(client ${CMAKE_THREAD_LIBS_INIT})
