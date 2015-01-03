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
    utils.h
    gladius-exception.h gladius-exception.cpp
)
