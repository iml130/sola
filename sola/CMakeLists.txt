cmake_minimum_required(VERSION 3.19)

#-------------------------------------------------------------------------------
# Project configuration
#-------------------------------------------------------------------------------
project(SOLA VERSION 0.1.0)
set(CMAKE_CXX_STANDARD 17)

cmake_policy(SET CMP0077 NEW)

message(STATUS "BUILD_TYPE: ${CMAKE_BUILD_TYPE}")
message(STATUS "SOLA VERSION: ${PROJECT_VERSION}")

option(SOLA_ENABLE_EXAMPLES "Enable examples" ON)

#-------------------------------------------------------------------------------
# Third-party dependencies
#-------------------------------------------------------------------------------

if(NOT TARGET minhton_core_node)
  message(FATAL_ERROR "MINHTON not provided from mono repository")
endif()

if(NOT TARGET natter_minhcast)
  message(FATAL_ERROR "NatterMinhcast not provided from mono repository")
endif()

if(NOT TARGET NetworkUDP)
  message(FATAL_ERROR "SolaNet NetworkUDP not provided from mono repository")
endif()

#-------------------------------------------------------------------------------
# Top-level components
#-------------------------------------------------------------------------------

add_subdirectory(src)

if(SOLA_ENABLE_EXAMPLES)
  add_subdirectory(examples)
endif()
