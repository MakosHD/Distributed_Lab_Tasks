cmake_minimum_required(VERSION 3.10.0)
include(FetchContent)

project(task3 VERSION 0.1.0 LANGUAGES C CXX)

add_executable(task3 src/main.cpp)

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET task3 PROPERTY CXX_STANDARD 20)
endif()

FetchContent_Declare(
  csv  
  GIT_REPOSITORY https://github.com/vincentlaucsb/csv-parser.git
  GIT_SHALLOW TRUE 
  GIT_TAG 5.3.0
) 

FetchContent_MakeAvailable(csv)

target_include_directories(task3 PRIVATE csv)

target_link_libraries(task3 csv)

