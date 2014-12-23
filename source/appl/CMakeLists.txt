#
# Copyright (c) 2014      Los Alamos National Security, LLC
#                         All rights reserved.
#
# This file is part of the Gladius project. See the LICENSE.txt file at the
# top-level directory of this distribution.
#

add_library(
    GladiusAppL
    appl-factory.h appl-factory.cpp
    appl.h appl.cpp
    appl-orte.h
)

include_directories(..)
