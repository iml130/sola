cmake_minimum_required(VERSION 3.19)

#-------------------------------------------------------------------------------
# Project configuration
#-------------------------------------------------------------------------------
project(sola_mono VERSION 1.0.0)
set(CMAKE_CXX_STANDARD 17)

# Set build type default to Debug
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "Debug")
endif()

#-------------------------------------------------------------------------------
# General third-party dependencies
#-------------------------------------------------------------------------------
add_subdirectory(third_party/Catch2)

#-------------------------------------------------------------------------------
# Add individual projects
#-------------------------------------------------------------------------------
add_subdirectory(solanet)
add_subdirectory(minhton)
add_subdirectory(natter)
add_subdirectory(sola)
