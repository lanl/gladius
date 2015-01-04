#
# Copyright (c) 2014-2015 Los Alamos National Security, LLC
#                         All rights reserved.
#
# This file is part of the Gladius project. See the LICENSE.txt file at the
# top-level directory of this distribution.
#

add_library(
    GladiusCore
    core.h
    gladius-package-info.h
    ret-codes.h
    env.h
    utils.h utils.cpp
    gladius-exception.h gladius-exception.cpp
)

include_directories(..)
# Used to help find the generated header during configure.
include_directories(${CMAKE_CURRENT_BINARY_DIR})

configure_file(gladius-paths.h.in gladius-paths.h)
set_source_files_properties(gladius-paths.h PROPERTIES GENERATED TRUE)
