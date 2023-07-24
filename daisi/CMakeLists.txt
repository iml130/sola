cmake_minimum_required(VERSION 3.19)

#-------------------------------------------------------------------------------
# Project configuration
#-------------------------------------------------------------------------------

project(DAISI VERSION 0.1.0)
set(CMAKE_CXX_STANDARD 17)

cmake_policy(VERSION 3.19)

option(DAISI_DISABLE_NETWORK_SIMULATION "Disable ns-3 network simulation (use ns-3 as discrete event simulator only)" OFF)

# Set default build type to Debug
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "Debug")
endif()

#-------------------------------------------------------------------------------
# Third-party dependencies
#-------------------------------------------------------------------------------

find_package(ns3 REQUIRED)

find_package(yaml-cpp REQUIRED)

find_package(SQLite3 REQUIRED)

find_package(Threads REQUIRED)

find_library(LIBUUID Names uuid PATHS /lib /lib64)
if(NOT LIBUUID)
  message(FATAL_ERROR "libuuid not found!")
endif()

# Add subprojects from monorepository folder
if(NOT EXISTS ../natter OR NOT EXISTS ../minhton OR NOT EXISTS ../sola)
    message(FATAL_ERROR "DAISI folder must be within monorepository folder")
endif()
add_subdirectory(../ ${CMAKE_CURRENT_BINARY_DIR}/sola_mono)

#-------------------------------------------------------------------------------
# Top-level components
#-------------------------------------------------------------------------------

add_subdirectory(src)

add_subdirectory(tests/unittests)

add_subdirectory(third_party)
