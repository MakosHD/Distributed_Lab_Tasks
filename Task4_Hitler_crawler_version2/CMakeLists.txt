cmake_minimum_required(VERSION 3.10.0)
include(FetchContent)

project(task4 VERSION 0.1.0 LANGUAGES C CXX)

add_executable(task4 src/main.cpp)

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET task4 PROPERTY CXX_STANDARD 20)
endif()

set(CPR_USE_SYSTEM_CURL ON)
FetchContent_Declare(
  cpr
  GIT_REPOSITORY https://github.com/libcpr/cpr.git
) 

FetchContent_MakeAvailable(cpr)

FetchContent_Declare(json URL https://github.com/nlohmann/json/releases/download/v3.12.0/json.tar.xz)

FetchContent_MakeAvailable(json)

target_include_directories(task4 PRIVATE cpr)

target_link_libraries(task4 PRIVATE nlohmann_json::nlohmann_json)
target_link_libraries(task4 PRIVATE cpr)

