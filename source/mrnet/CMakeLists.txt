#
# Copyright (c) 2014-2015 Los Alamos National Security, LLC
#                         All rights reserved.
#
# This file is part of the Gladius project. See the LICENSE.txt file at the
# top-level directory of this distribution.
#

add_library(
    GladiusMRNetFE
    mrnet-fe.cpp mrnet-fe.h
)

include_directories(..)
# Used to help find the generated header during configure.
include_directories(${CMAKE_CURRENT_BINARY_DIR}/..)

target_link_libraries(
    GladiusMRNetFE
    ${GLADIUS_MRNET_LIBS}
)

# Generate header from .in.
# @see cmake/FindMRNet.cmake for more details about GLADIUS_MRNET_HOME.
set(GLADIUS_MRNET_PATH ${GLADIUS_MRNET_HOME})
configure_file(mrnet-paths.h.in mrnet-paths.h)
set_source_files_properties(mrnet-paths.h PROPERTIES GENERATED TRUE)
