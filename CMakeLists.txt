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
include(cmake/FindLLDB.cmake)
include(cmake/EchoFinalSetup.cmake)
# Setup build type stuff
buildTypeSetup()
# Get the necessary base CXX flags
getCXXFlags(GLADIUS_BASE_CXX_FLAGS)
# Append to CMAKE_CXX_FLAGS
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${GLADIUS_BASE_CXX_FLAGS}")
findLLDB(LLDB_CXX_FLAGS LLDB_LD_FLAGS)
# add top-level source directory
add_subdirectory(source)
# show final configuration
echoFinalSetup()
