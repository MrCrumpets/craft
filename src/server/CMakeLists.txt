cmake_minimum_required(VERSION 3.2)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

include_directories(/home/nathan/Downloads/asio-1.10.6/include)
find_package(Threads)

set(SOURCE_FILES main.cpp)

add_executable(server ${SOURCE_FILES} ../shared/chat_message.hpp)
target_link_libraries(server ${CMAKE_THREAD_LIBS_INIT})
