cmake_minimum_required(VERSION 3.19)

#-------------------------------------------------------------------------------
# Project configuration
#-------------------------------------------------------------------------------

project(natter VERSION 0.1.0)
set(CMAKE_CXX_STANDARD 17)

cmake_policy(SET CMP0077 NEW)

# Set build type default to Debug
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "Debug")
endif()

message(STATUS "BUILD_TYPE: ${CMAKE_BUILD_TYPE}")
message(STATUS "NATTER VERSION: ${PROJECT_VERSION}")

option(NATTER_ENABLE_CLANGTIDY "Execute Clang-Tidy on build" OFF)
option(NATTER_ENABLE_LOGGING "Enable logging system" ON)
option(NATTER_ENABLE_EXAMPLES "Enable examples" ON)
option(NATTER_ENABLE_TESTS "Enable tests" ON)
option(NATTER_USE_SOLANET_SUBMODULE "Use SolaNet from third_party submodule" ON)
option(NATTER_BUILD_SINGLE_TEST_BINARY "Build all tests into a single binary" ON)

list(APPEND CMAKE_MODULE_PATH ${natter_SOURCE_DIR}/build_tools)

#-------------------------------------------------------------------------------
# Third-party dependencies
#-------------------------------------------------------------------------------

find_package(Threads REQUIRED)

if(NOT TARGET NetworkUDP)
  message(FATAL_ERROR "SolaNet NetworkUDP not provided from mono repository")
endif()

if(NATTER_ENABLE_TESTS AND NOT TARGET Catch2WithMain)
  message(FATAL_ERROR "Catch2 target not available!")
endif()

#-------------------------------------------------------------------------------
# Top-level components
#-------------------------------------------------------------------------------

add_subdirectory(src)

if(NATTER_ENABLE_EXAMPLES)
  add_subdirectory(examples)
endif()

if(NATTER_ENABLE_TESTS)
  add_subdirectory(tests)
endif()
