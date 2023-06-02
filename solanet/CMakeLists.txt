cmake_minimum_required(VERSION 3.19)

#-------------------------------------------------------------------------------
# Project configuration
#-------------------------------------------------------------------------------

project(SolaNet VERSION 0.1.0)
set(CMAKE_CXX_STANDARD 17)

cmake_policy(SET CMP0077 NEW)

option(SOLANET_ENABLE_EXAMPLES "Enable examples" ON)
option(SOLANET_ENABLE_TESTS "Enable tests" ON)

#-------------------------------------------------------------------------------
# Third-party dependencies
#-------------------------------------------------------------------------------

find_package(Threads REQUIRED)

add_subdirectory(build_tools/third_party/asio/ EXCLUDE_FROM_ALL)
add_subdirectory(build_tools/third_party/cereal EXCLUDE_FROM_ALL)

if(SOLANET_ENABLE_TESTS AND NOT TARGET Catch2::Catch2WithMain)
  message(FATAL_ERROR "Catch2 not provided from monorepository")
endif()

#-------------------------------------------------------------------------------
# Top-level components
#-------------------------------------------------------------------------------

add_subdirectory(src)

if(SOLANET_ENABLE_EXAMPLES)
  add_subdirectory(examples)
endif()

if(SOLANET_ENABLE_TESTS)
  add_subdirectory(tests)
endif()
