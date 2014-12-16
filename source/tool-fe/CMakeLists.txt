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
)

include_directories(..)

target_link_libraries(
    GladiusToolBE
)
