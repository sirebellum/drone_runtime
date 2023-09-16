cmake_minimum_required(VERSION 3.2)
project(fc_firmware)

# Get source files
file(GLOB_RECURSE SOURCE_FILES *.cpp)

# Add executable
add_executable(fc_firmware ${SOURCE_FILES})

# Link
target_link_libraries(fc_firmware tvm ${OpenCV_LIBS})

# Move executable to top build dir
set_target_properties(fc_firmware PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
