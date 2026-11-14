
cmake_minimum_required(VERSION 3.16)

project(AccessWayAPI LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(FetchContent)

# HTTP server library
FetchContent_Declare(
    httplib
    GIT_REPOSITORY https://github.com/yhirose/cpp-httplib.git
    GIT_TAG v0.18.3
)

# JSON library
FetchContent_Declare(
    nlohmann_json
    GIT_REPOSITORY https://github.com/nlohmann/json.git
    GIT_TAG v3.12.0
)

FetchContent_MakeAvailable(httplib nlohmann_json)

# Include existing AccessWay backend source files.
file(GLOB BACKEND_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/../*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/../common/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/../member3/*.cpp"
)

# Exclude existing demo/test entry points.
list(FILTER BACKEND_SOURCES EXCLUDE REGEX "/main\\.cpp$")

add_executable(accessway_api
    server.cpp
    ${BACKEND_SOURCES}
)

target_include_directories(accessway_api PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/.."
    "${CMAKE_CURRENT_SOURCE_DIR}/../common"
    "${CMAKE_CURRENT_SOURCE_DIR}/../member3"
)

target_link_libraries(accessway_api PRIVATE
    httplib::httplib
    nlohmann_json::nlohmann_json
)
