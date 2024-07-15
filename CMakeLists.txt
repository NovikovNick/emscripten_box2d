cmake_minimum_required(VERSION 3.10)
project ("emscripten_box2d"
    VERSION 1.0.0
    DESCRIPTION "Just a sample of compiled box2d with emscripten"
    LANGUAGES CXX)

if (NOT EMSCRIPTEN)
	message(FATAL_ERROR "Only emscripten compiler supported")
endif()

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}")

add_definitions(-std=c++20 -O2)

add_subdirectory ("third_parties/box2d/src")

set(CMAKE_EXECUTABLE_SUFFIX ".wasm.js")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/html")

add_executable(sample "src/main.cc")
target_link_libraries(sample 
    PRIVATE
    third-parties::box2d)
set_target_properties(sample
    PROPERTIES 
        LINK_FLAGS "-lembind -s 'EXPORTED_RUNTIME_METHODS=[ccall]' -s 'EXPORTED_FUNCTIONS=[_free, _malloc]' -sSTACK_SIZE=5MB")

message(STATUS "")
message(STATUS "Configuration summary:")
message(STATUS "")
message(STATUS "  Compiler ...........................: ${CMAKE_CXX_COMPILER_ID}")
message(STATUS "")
