#
# Copyright (c) 2014-2015 Los Alamos National Security, LLC
#                         All rights reserved.
#
# This file is part of the Gladius project. See the LICENSE.txt file at the
# top-level directory of this distribution.
#

macro(BUILD_TYPE_SETUP)
    SET_BUILD_TYPE_BOOLS()
endmacro()

macro(SET_BUILD_TYPE_BOOLS)
    # Normalize build type string
    string(TOLOWER "${CMAKE_BUILD_TYPE}" GLADIUS_BUILD_TYPE_STRING)
    # Set defaults
    set(GLADIUS_BUILD_TYPE_NONE 0)
    set(GLADIUS_BUILD_TYPE_DEBUG 0)
    set(GLADIUS_BUILD_TYPE_REL 0)
    set(GLADIUS_BUILD_TYPE_REL_WDBI 0)
    # Empty string in this case, let's take care of that... Debug build mode is
    # the default if nothing is explicitly provided.
    if (GLADIUS_BUILD_TYPE_STRING STREQUAL "")
        set(GLADIUS_BUILD_TYPE_STRING "debug")
    endif()
    if(GLADIUS_BUILD_TYPE_STRING STREQUAL "debug")
        set(GLADIUS_BUILD_TYPE_DEBUG 1)
    elseif(GLADIUS_BUILD_TYPE_STRING STREQUAL "release")
        set(GLADIUS_BUILD_TYPE_REL 1)
    elseif(GLADIUS_BUILD_TYPE_STRING STREQUAL "relwithdebinfo")
        set(GLADIUS_BUILD_TYPE_REL_WDBI 1)
    endif()
endmacro()
