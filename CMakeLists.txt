# Copyright(c) 2023 Tsingmicro NPU model  project

# ---------------------------------------------------------------------------------------
# Start LT model project
# ---------------------------------------------------------------------------------------
SET( CMAKE_CXX_FLAGS "-std=c++17 -O2 -g -fpermissive -pthread -fPIC -m64")
CMAKE_MINIMUM_REQUIRED(VERSION 3.22)

# ---------------------------------------------------------------------------------------
# Set the output directory for shared libraries (e.g., .so, .dll files)
# ---------------------------------------------------------------------------------------
SET(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# ---------------------------------------------------------------------------------------
# Set the output directory for static libraries (e.g., .a, .lib files)
# ---------------------------------------------------------------------------------------
SET(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# ---------------------------------------------------------------------------------------
# Set the output directory for executable files
# ---------------------------------------------------------------------------------------
SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY  ${CMAKE_BINARY_DIR}/bin)

# ---------------------------------------------------------------------------------------
# Set install direcotory
# ---------------------------------------------------------------------------------------
SET(CMAKE_INSTALL_PREFIX ${CMAKE_BINARY_DIR} CACHE PATH "Installation Directory" FORCE)

# ---------------------------------------------------------------------------------------
# Set DV direcotory
# ---------------------------------------------------------------------------------------
SET(CMAKE_DV_DIRECTORY  ${CMAKE_BINARY_DIR}/dv)

# ---------------------------------------------------------------------------------------
# Set export static library 
# --------------------------------------------------------------------------------------

IF (POLICY CMP0048)
  cmake_policy(SET CMP0048 NEW)
ENDIF (POLICY CMP0048)

PROJECT(Explorer-LT-model)
SET (LT_MODEL_VERSION_MAJOR 1)
SET (LT_MODEL_VERSION_MINOR 0)
SET (LT_MODEL_VERSION_PATCH 1)

# ---------------------------------------------------------------------------------------
# Set  static library and install direcotory 
# ---------------------------------------------------------------------------------------
SET(NPU_LIBRARY_DIRECTORY  ${CMAKE_BINARY_DIR}/npu/lib)
SET(NPUT_HEAD_DIRECTORY  ${CMAKE_BINARY_DIR}/npu/include)

# ---------------------------------------------------------------------------------------
# Set module library name 
# ---------------------------------------------------------------------------------------
SET(CACHE_LIBRARY tx82_npu_cache)

# ---------------------------------------------------------------------------------------
# Show compile sysytem message
# ---------------------------------------------------------------------------------------
MESSAGE(STATUS "CMAKE_LIBRARY_OUTPUT_DIRECTORY : " ${CMAKE_LIBRARY_OUTPUT_DIRECTORY})
MESSAGE(STATUS "CMAKE_RUNTIME_OUTPUT_DIRECTORY : " ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
MESSAGE(STATUS "CMAKE_SOURCE_DIR: " ${CMAKE_SOURCE_DIR})
MESSAGE(STATUS "CMAKE_BINARY_DIR: " ${CMAKE_BINARY_DIR})
MESSAGE(STATUS "CMAKE_DV_DIRECTORY: " ${CMAKE_DV_DIRECTORY})
MESSAGE(STATUS "CMAKE_ARCHIVE_OUTPUT_DIRECTORY: " ${CMAKE_ARCHIVE_OUTPUT_DIRECTORY})
MESSAGE(STATUS "CMAKE_INSTALL_PREFIX: " ${CMAKE_INSTALL_PREFIX})

# ---------------------------------------------------------------------------------------
# Add subdirectory 
# ---------------------------------------------------------------------------------------
#ADD_SUBDIRECTORY(top)
#ADD_SUBDIRECTORY(tags)
#ADD_SUBDIRECTORY(replacement)
#ADD_SUBDIRECTORY(prefetch)
#ADD_SUBDIRECTORY(indexing)
#ADD_SUBDIRECTORY(base)

FILE(GLOB_RECURSE CACHE_SRC_FILES 
    "base/*.cpp"
    "cache/*.cpp"
    "indexing/*.cpp"
    "prefetch/*.cpp"
    "mshr/*.cpp"
    "replaceable/*.cpp"
    "tags/*.cpp"
    "write/*.cpp"
    "top/*.cpp"
    "memory/*.cpp")

FILE(GLOB_RECURSE CACHE_INC_FILES 
    "base/*.h" 
    "cache/*.h" 
    "indexing/*.h" 
    "prefetch/*.h" 
    "replaceable/*.h" 
    "tags/*.h" 
    "mshr/*.h"
    "write/*.h"
    "param/*.h"
    "top/*.h"
    "memory/*.h")

# ---------------------------------------------------------------------------------------
# Set the library information for compiling output.
# ---------------------------------------------------------------------------------------
ADD_LIBRARY(${CACHE_LIBRARY} STATIC 
            ${CACHE_SRC_FILES} 
            ${CACHE_INC_FILES})

SET_TARGET_PROPERTIES(${CACHE_LIBRARY} PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_ARCHIVE_OUTPUT_DIRECTORY}
)

# ---------------------------------------------------------------------------------------
# include  3rd_party and npu head  files
# ---------------------------------------------------------------------------------------
INCLUDE_DIRECTORIES(
    ${CMAKE_SOURCE_DIR}/
    ${CMAKE_SOURCE_DIR}/lib/spdlog/include/
    ${CMAKE_SOURCE_DIR}/lib/systemc/include/
    ${CMAKE_SOURCE_DIR}/base/include/
    ${CMAKE_SOURCE_DIR}/params/
    ${CMAKE_SOURCE_DIR}/cache/
    ${CMAKE_SOURCE_DIR}/indexing/
     ${CMAKE_SOURCE_DIR}/memory/
    ${CMAKE_SOURCE_DIR}/prefetch/
    ${CMAKE_SOURCE_DIR}/replaceable/
    ${CMAKE_SOURCE_DIR}/tags/
    ${CMAKE_SOURCE_DIR}/top/
)

LINK_DIRECTORIES (
    ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}
    ${CMAKE_SOURCE_DIR}/lib/spdlog/
    ${CMAKE_SOURCE_DIR}/lib/systemc/lib/
)

LINK_LIBRARIES (
    ${CACHE_LIBRARY}
    systemc
)

ADD_EXECUTABLE(dv_cache_case 
    main.cpp
)
