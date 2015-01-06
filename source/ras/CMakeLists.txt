#
# Copyright (c) 2014-2015 Los Alamos National Security, LLC
#                         All rights reserved.
#
# This file is part of the Gladius project. See the LICENSE.txt file at the
# top-level directory of this distribution.
#

add_library(
    GladiusRAS
    ras.cpp ras.h
)

target_link_libraries(
    GladiusRAS
    ${GLADIUS_LMON_LIBS}
)

include_directories(..)
