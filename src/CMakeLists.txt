cmake_minimum_required(VERSION 3.2)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++1y -stdlib=libc++")
find_package(Threads)

add_definitions(-DBOOST_ASIO_ENABLE_HANDLER_TRACKING)

add_executable(node main.cpp node/raft_network.cpp node/raft_node.cpp)
target_link_libraries(node ${CMAKE_THREAD_LIBS_INIT})
