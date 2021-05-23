cmake_minimum_required(VERSION 3.15 FATAL_ERROR)
project(dotEngine C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED True)

# Include external dependencies
include(FetchContent)
# set(FETCHCONTENT_QUIET OFF)

FetchContent_Declare(
  inih
  GIT_REPOSITORY "https://github.com/benhoyt/inih"
  GIT_TAG "4f251f0ff766052c342823dfa52a04f486cc4f94")

FetchContent_Declare(
  cJSON
  GIT_REPOSITORY "https://github.com/DaveGamble/cJSON"
  GIT_TAG "324a6ac9a9b285ff7a5a3e5b2071e3624b94f2db")

# Disable building of tests and utils for cJSON
set(ENABLE_CJSON_TEST
    OFF
    CACHE INTERNAL "Disable tests")
set(ENABLE_CJSON_UTILS
    OFF
    CACHE INTERNAL "Disable Utils")

FetchContent_MakeAvailable(inih cJSON)
include_directories(${inih_SOURCE_DIR} ${cJSON_SOURCE_DIR})

# Add src files
set(SRC_FILES
    ${PROJECT_SOURCE_DIR}/src/http.c
    ${PROJECT_SOURCE_DIR}/src/main.c
    ${PROJECT_SOURCE_DIR}/src/parser.c
    ${PROJECT_SOURCE_DIR}/src/username.c
    # External dependencies, inih doesn't use cmake :(
    ${inih_SOURCE_DIR}/ini.c)
add_executable(${PROJECT_NAME} ${SRC_FILES})

# cURL
find_package(CURL REQUIRED)
include_directories(${CURL_INCLUDE_DIR})

# Linking
target_link_libraries(${PROJECT_NAME} curl cjson)

# Compiler warnings
if(MSVC)
  target_compile_options(${PROJECT_NAME} PRIVATE /W4 /WX)
else()
  target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -pedantic
                                                 -Werror)
endif()
