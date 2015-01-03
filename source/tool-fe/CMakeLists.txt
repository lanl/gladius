#
# Copyright (c) 2014      Los Alamos National Security, LLC
#                         All rights reserved.
#
# This file is part of the Gladius project. See the LICENSE.txt file at the
# top-level directory of this distribution.
#

add_library(
    GladiusToolFE
    tool-fe.cpp tool-fe.h
    lmon.cpp lmon.h
)

include_directories(..)
include_directories(${CMAKE_CURRENT_BINARY_DIR})

target_link_libraries(
    GladiusToolFE
    ${GLADIUS_LMON_LIBS}
)

# Generate header from .in.
# @see cmake/FindLaunchMON.cmake for more details about GLADIUS_LMON_HOME.
#set(GLADIUS_TOOL_FE_LMON_PATH ${GLADIUS_LMON_HOME})
#configure_file(lmon-path.h.in lmon-path.h)
#set_source_files_properties(lmon-path.h PROPERTIES GENERATED TRUE)
