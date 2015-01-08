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

target_link_libraries(
    GladiusMRNetFE
    ${GLADIUS_MRNET_LIBS}
)
