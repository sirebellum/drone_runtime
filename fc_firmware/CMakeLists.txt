cmake_minimum_required(VERSION 3.2)

project(fc_firmware)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -O3")

# Find projects
find_package(tvm REQUIRED)
find_package(OpenCV REQUIRED)

# Add include directories
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
include_directories(${OpenCV_INCLUDE_DIRS})
include_directories(${TVM_INCLUDE_DIRS})

# Manually add tvm includes and libs if on mac
if(APPLE)
    include_directories(/usr/local/include)
    link_directories(/usr/local/lib)
endif()

# Add subdirectories
add_subdirectory(src)
