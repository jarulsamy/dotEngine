cmake_minimum_required(VERSION 3.15 FATAL_ERROR)
project(
  dotEngine
  VERSION 2.0
  DESCRIPTION
    "An overkill way to retrieve (and cache) data from the GitHub API."
  LANGUAGES C CXX)
configure_file(src/config.h.in config.h)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Include external dependencies
include(FetchContent)
# set(FETCHCONTENT_QUIET OFF)

FetchContent_Declare(
  inih
  GIT_REPOSITORY "https://github.com/benhoyt/inih"
  GIT_TAG "4f251f0ff766052c342823dfa52a04f486cc4f94")

FetchContent_MakeAvailable(inih)
include_directories(${inih_SOURCE_DIR})
include_directories(${inih_SOURCE_DIR}/cpp)

# inih options
add_definitions(-DINI_ALLOW_MULTILINE=0)

# Add src files
set(SRC_FILES
    ${PROJECT_SOURCE_DIR}/src/main.cpp
    ${PROJECT_SOURCE_DIR}/src/userconfig.cpp
    ${PROJECT_SOURCE_DIR}/src/gh.cpp
    # External dependencies, inih doesn't use cmake
    ${inih_SOURCE_DIR}/ini.c
    ${inih_SOURCE_DIR}/cpp/INIReader.cpp)
add_executable(${PROJECT_NAME} ${SRC_FILES})

# Include for config
target_include_directories(${PROJECT_NAME} PRIVATE "${PROJECT_BINARY_DIR}")

# cURL
find_package(CURL REQUIRED)
include_directories(${CURL_INCLUDE_DIR})

# Boost
find_package(Boost 1.67.0 REQUIRED COMPONENTS json)
include_directories(${BOOST_INCLUDE_DIR})

# Linking
target_link_libraries(${PROJECT_NAME} curl Boost::json)

# Compiler warnings, platform specific.
add_compile_options("$<$<CONFIG:RELEASE>:-W;-Wall;-O3;-pedantic>")
add_compile_options("$<$<CONFIG:DEBUG>:-W;-Wall;-O0;-g;-pedantic>")
