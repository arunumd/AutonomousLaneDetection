#Set your CMake Minimum requirement
cmake_minimum_required(VERSION 3.2.1)

#Define your project
project (Project1)

#Find required packages
find_package(OpenCV REQUIRED)
find_package(Boost COMPONENTS system filesystem REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})

# Add project cmake modules to path.
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${PROJECT_SOURCE_DIR}/cmake)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 11)
set(NAME_SRC app/main.cpp app/Files.cpp app/Cleaner.cpp app/Thresholder.cpp app/LanesMarker.cpp app/RegionMaker.cpp)
set(NAME_HEADERS include/Files.hpp include/Cleaner.hpp include/Thresholder.hpp include/LanesMarker.hpp include/RegionMaker.hpp)

include(CMakeToolsHelpers OPTIONAL)
include_directories(${CMAKE_SOURCE_DIR}/include)

#Add sub-directories
add_subdirectory(app)

#Add executables
add_executable(Project1 ${NAME_SRC} ${NAME_HEADERS})

#Link libraries
target_link_libraries(Project1 ${OpenCV_LIBS} ${Boost_LIBRARIES})
