#
# Copyright (c) 2014-2015 Los Alamos National Security, LLC
#                         All rights reserved.
#
# This file is part of the Gladius project. See the LICENSE.txt file at the
# top-level directory of this distribution.
#

add_library(
    GladiusToolBE
    tool-be.cpp tool-be.h
)

add_executable(
    gladius-toold
    gladius-toold.cpp
)

target_link_libraries(
    gladius-toold
    GladiusToolBE
    ${GLADIUS_LMON_LIBS_BE}
)

install(TARGETS gladius-toold DESTINATION bin)

include_directories(..)
