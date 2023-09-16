cmake_minimum_required(VERSION 3.2)

project(fc_firmware)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -O3")

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

# Find projects
find_package(tvm REQUIRED)
find_package(OpenCV REQUIRED)

# Add subdirectories
add_subdirectory(src)
