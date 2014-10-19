#
# Copyright (c) 2014      Los Alamos National Security, LLC
#                         All rights reserved.
#
# This file is part of the Gladius project. See the LICENSE.txt file at the
# top-level directory of this distribution.
#

# The Gladius Debugger BackEnd library
add_library(
    GladiusTerm
    term-cmds.h
    term.h term.cpp
)

include_directories(..)

# TODO Add proper FindLibEdit configury.
target_link_libraries(
    GladiusTerm
    edit
)
