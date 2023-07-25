cmake_minimum_required(VERSION 3.19)

#-------------------------------------------------------------------------------
# Project configuration
#-------------------------------------------------------------------------------

project(MINHTON VERSION 0.1.0)
set(CMAKE_CXX_STANDARD 17)

cmake_policy(SET CMP0077 NEW)

option(MINHTON_ENABLE_TESTS "Enable tests" ON)
option(MINHTON_BUILD_SINGLE_TEST_BINARY "Build all tests into a single binary" ON)


message(STATUS "BUILD_TYPE: ${CMAKE_BUILD_TYPE}")
message(STATUS "MINHTON VERSION: ${PROJECT_VERSION}")
message(STATUS "BUILD TESTS: ${MINHTON_ENABLE_TESTS}")

list(APPEND CMAKE_MODULE_PATH ${MINHTON_SOURCE_DIR}/build_tools)

#-------------------------------------------------------------------------------
# Third-party dependencies
#-------------------------------------------------------------------------------

find_package(yaml-cpp REQUIRED)

find_package(Threads REQUIRED)

find_library(LIBEVENT NAMES event)
if(NOT LIBEVENT)
  message(FATAL_ERROR "libevent not found!")
endif()

find_library(LIBEVENT_PTHREADS NAMES event_pthreads)
if(NOT LIBEVENT_PTHREADS)
  message(FATAL_ERROR "libevent_pthreads not found!")
endif()

add_subdirectory(build_tools/third_party/peglib)
add_subdirectory(build_tools/third_party/fsmlite)

if(MINHTON_ENABLE_TESTS AND NOT TARGET Catch2::Catch2WithMain)
  message(FATAL_ERROR "Catch2 not provided from monorepository")
endif()

if(NOT TARGET Cereal)
  message(FATAL_ERROR "Cereal not provided from monorepository")
endif()

if(NOT TARGET NetworkUDP)
  message(FATAL_ERROR "SolaNet NetworkUDP not provided from mono repository")
endif()

#-------------------------------------------------------------------------------
# Top-level components
#-------------------------------------------------------------------------------

add_subdirectory(src)

if(MINHTON_ENABLE_TESTS)
  #enable_testing()
  add_subdirectory(tests)
endif()
