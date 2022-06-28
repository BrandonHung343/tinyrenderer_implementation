cmake_minimum_required (VERSION 2.8)
project(tinyrenderer)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(OpenMP)
if(OPENMP_FOUND)
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
endif()


# You have to set these variables as Windows environment variables:
# EIGEN3_INCLUDE_DIR <- %EIGEN3_ROOT%
# EIGEN3_DIR <- %EIGEN3_ROOT%\cmake
#
# EIGEN3_INCLUDE_DIR: variable needed for file %EIGEN3_ROOT%/cmake/FindEigen3.cmake
#
# CMAKE_MODULE_PATH: Search path for the module Eigen3 to be loaded by find_package
#
SET( EIGEN3_INCLUDE_DIR "$ENV{EIGEN3_INCLUDE_DIR}" )
SET( CMAKE_MODULE_PATH "$ENV{EIGEN3_DIR}" )

find_package( Eigen3 REQUIRED )
# include_directories is needed for the compiler to know where looking for Eigen3 header files to be included 
include_directories( "$ENV{EIGEN3_INCLUDE_DIR}" )

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

file(GLOB SOURCES *.h *.cpp)

add_executable(${PROJECT_NAME} ${SOURCES})
# target_link_libraries(${PROJECT_NAME} ${Eigen_LIBRARIES})

