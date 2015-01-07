#
# Copyright (c) 2014-2015 Los Alamos National Security, LLC
#                         All rights reserved.
#
# This file is part of the Gladius project. See the LICENSE.txt file at the
# top-level directory of this distribution.
#

add_library(
    GladiusToolCommon
    tool-common.h tool-common.cpp
)

include_directories(..)
# Used to help find the generated header during configure.
include_directories(${CMAKE_CURRENT_BINARY_DIR})
