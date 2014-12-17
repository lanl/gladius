#
# Copyright (c) 2014      Los Alamos National Security, LLC
#                         All rights reserved.
#
# This file is part of the Gladius project. See the LICENSE.txt file at the
# top-level directory of this distribution.
#

# cmake -LAH to list all project options
project(Gladius)
# Set minimum required version
cmake_minimum_required(VERSION 2.8.11)
# Project configury includes
include(cmake/BuildType.cmake)
include(cmake/CXXOptions.cmake)
include(cmake/FindThreads.cmake)
include(cmake/FindLaunchMON.cmake)
include(cmake/FindMRNet.cmake)
include(cmake/FindLLDB.cmake)
include(cmake/EchoFinalSetup.cmake)
include(cmake/DocOptions.cmake)

################################################################################
# Versioning
################################################################################
set(VERSION_MAJOR   0   CACHE STRING "Project major version number.")
set(VERSION_MINOR   1   CACHE STRING "Project minor version number.")
set(VERSION_PATCH   0   CACHE STRING "Project patch version number.")
mark_as_advanced(VERSION_MAJOR VERSION_MINOR VERSION_PATCH)

################################################################################
# Setup build type stuff
################################################################################
buildTypeSetup()

################################################################################
# Set documentation configury.
################################################################################
docSetup()

################################################################################
# Threading setup
################################################################################
findThreads(GLADIUS_PTHREAD_C_FLAGS GLADIUS_PTHREAD_LINK_LIBS)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${GLADIUS_PTHREAD_C_FLAGS}")

################################################################################
# Get the necessary base CXX flags
################################################################################
getCXXFlags(GLADIUS_BASE_CXX_FLAGS)
# Append to CMAKE_CXX_FLAGS
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${GLADIUS_BASE_CXX_FLAGS}")

################################################################################
# LLDB setup
################################################################################
findLLDB(GLADIUS_LLDB_INCLUDE_DIRS GLADIUS_LLDB_LIB_DIRS GLADIUS_LLDB_LIBS)
include_directories(${GLADIUS_LLDB_INCLUDE_DIRS})
# XXX The use of this may be incorrect. Question: does this blast or append to
# the list of link directories.
link_directories(${GLADIUS_LLDB_LIB_DIRS})
#target_link_libraries use where needed with ${GLADIUS_LLDB_LIBS}

################################################################################
# LaunchMON setup
################################################################################
findLMON(GLADIUS_LMON_INCLUDE_DIRS GLADIUS_LMON_LIB_DIRS GLADIUS_LMON_LIBS)
include_directories(${GLADIUS_LMON_INCLUDE_DIRS})
link_directories(${GLADIUS_LMON_LIB_DIRS})
#target_link_libraries use where needed with ${GLADIUS_LMON_LIBS}

################################################################################
# MRNet setup
################################################################################
findMRNet(GLADIUS_MRNET_INCLUDE_DIRS GLADIUS_MRNET_LIB_DIRS GLADIUS_MRNET_LIBS)
include_directories(${GLADIUS_MRNET_INCLUDE_DIRS})
link_directories(${GLADIUS_MRNET_LIB_DIRS})
#target_link_libraries use where needed with ${GLADIUS_MRNET_LIBS}

################################################################################
# add top-level source directory
################################################################################
add_subdirectory(source)

################################################################################
# show final configuration
################################################################################
echoFinalSetup()
