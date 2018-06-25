cmake_minimum_required(VERSION 3.1.0 FATAL_ERROR)
project( lane_detect CXX)
find_package( OpenCV REQUIRED )
find_package(Boost COMPONENTS system filesystem REQUIRED)
include_directories(${Boost_INCLUDE_DIR})
link_directories(${Boost_LIBRARY_DIR})

set (CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(Boost_INCLUDE_DIR /usr/local/src/boost_1_58_0)
set(Boost_LIBRARY_DIR /usr/local/src/boost_1_58_0/stage/lib)

add_executable(lane_detect src/lane_detect.cpp)
target_link_libraries(lane_detect PRIVATE ${OpenCV_LIBS} ${Boost_LIBRARIES})
