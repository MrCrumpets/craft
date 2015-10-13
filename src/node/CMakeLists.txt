cmake_minimum_required(VERSION 3.2)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++1y")

include_directories(/home/nathan/Downloads/asio-1.10.6/include)
include_directories(/home/nathan/Documents/code/cereal/include/)
find_package(Threads)

set(SOURCE_FILES ../main.cpp)
add_definitions(-DBOOST_ASIO_ENABLE_HANDLER_TRACKING)

add_executable(node ${SOURCE_FILES} raft_network.cpp raft_node.cpp)
target_link_libraries(node ${CMAKE_THREAD_LIBS_INIT})
