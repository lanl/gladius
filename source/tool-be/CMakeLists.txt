#
# Copyright (c) 2014      Los Alamos National Security, LLC
#                         All rights reserved.
#
# This file is part of the Gladius project. See the LICENSE.txt file at the
# top-level directory of this distribution.
#

add_executable(
    gladius-toold
    tool-be.cpp tool-be.h
)

install(TARGETS gladius-toold DESTINATION bin)

include_directories(..)
