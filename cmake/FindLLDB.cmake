#
# Copyright (c) 2014-2015 Los Alamos National Security, LLC
#                         All rights reserved.
#
# This FILE is part of the Gladius project. See the LICENSE.txt FILE at the
# top-level directory of this distribution.
#

set(
    GLADIUS_LLDB_HOME
    "" CACHE FILEPATH
    "The base directory of the LLDB installation that will be used to build
    gladius. (Required)"
)

function(FIND_LLDB OUT_INC_DIRS OUT_LINK_DIRS OUT_LINK_LIBS)
    if (GLADIUS_LLDB_HOME STREQUAL "")
        message(FATAL_ERROR "GLADIUS_LLDB_HOME not defined. Cannot continue.")
    endif()
    set(prefix ${GLADIUS_LLDB_HOME})
    message(STATUS "Looking for LLDB in ${prefix}")
    message(STATUS "Making sure ${prefix} is a valid path...")
    if(NOT EXISTS ${prefix})
        message(FATAL_ERROR "${prefix} does not exist. Cannot continue")
    endif()
    message(STATUS "Making sure ${prefix} has some FILEs we need...")
    FILE(GLOB HEADERS_WE_NEED
        "${prefix}/include/lldb/API/LLDB.h"
    )
    foreach(FILE ${HEADERS_WE_NEED})
        message(STATUS "Looking for ${FILE}")
        if(NOT EXISTS "${FILE}")
            message(FATAL_ERROR "Cannot find ${FILE}. Cannot continue.")
        endif()
    endforeach()
    # Add any needed libraries here
    set(LIB_NAMES
        "lldb"
        "lldbAPI"
    )
    set(LIBS_WE_NEED)
    foreach(LIB_NAME ${LIB_NAMES})
        message(STATUS "Looking for lib${LIB_NAME}")
        file(GLOB LIB_WE_NEED
            "${prefix}/lib/lib${LIB_NAME}.*"
        )
        list(APPEND LIBS_WE_NEED ${LIB_WE_NEED})
        list(LENGTH LIB_WE_NEED NUM_FOUND)
        if(NUM_FOUND EQUAL 0)
            message(
                FATAL_ERROR
                "Cannot find necessary library: lib${LIB_NAME}.  "
                "Cannot continue."
            )
        endif()
    endforeach()
    # If we are here, then all is well. Return the outs.
    set(${OUT_INC_DIRS} "${prefix}/include" PARENT_SCOPE)
    set(${OUT_LINK_DIRS} "${prefix}/lib" PARENT_SCOPE)
    set(${OUT_LINK_LIBS} ${LIB_NAMES} PARENT_SCOPE)
endfunction()
