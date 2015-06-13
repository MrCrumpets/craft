cmake_minimum_required(VERSION 3.2)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

include_directories(/home/nathan/Downloads/asio-1.10.6/include)
find_package(Threads)

set(SOURCE_FILES main.cpp)
add_definitions(-DBOOST_ASIO_ENABLE_HANDLER_TRACKING)

add_executable(node ${SOURCE_FILES} Connection.cpp Connection.h NodeState/NodeState.cpp NodeState/Follower.cpp NodeState/Candidate.cpp NodeState/Candidate.h NodeState/Leader.cpp NodeState/Leader.h)
target_link_libraries(node ${CMAKE_THREAD_LIBS_INIT})
