#
# Copyright (c) 2014      Los Alamos National Security, LLC
#                         All rights reserved.
#
# This file is part of the Gladius project. See the LICENSE.txt file at the
# top-level directory of this distribution.
#

set(
    GLADIUS_LLDB_HOME
    "" CACHE FILEPATH
    "The base directory of the LLDB installation that will be used to build
    gladius. (Required)"
)

function(findLLDB outIncludeDirs outLinkDirs outLinkLibs)
    if (GLADIUS_LLDB_HOME STREQUAL "")
        message(FATAL_ERROR "GLADIUS_LLDB_HOME not defined. Cannot continue.")
    endif()
    set(prefix ${GLADIUS_LLDB_HOME})
    message(STATUS "Looking for LLDB in ${prefix}")
    message(STATUS "Making sure ${prefix} is a valid path...")
    if(NOT EXISTS ${prefix})
        message(FATAL_ERROR "${prefix} does not exist. Cannot continue")
    endif()
    message(STATUS "Making sure ${prefix} has some files we need...")
    # FIXME add proper, os-specific extensions for libraries
    set(fileWeNeed
        "include/lldb/API/LLDB.h"
        "lib/liblldb.dylib"
    )
    foreach (file ${fileWeNeed})
        message(STATUS "Looking for ${file} in ${prefix}")
        if(NOT EXISTS "${prefix}/${file}")
            message(FATAL_ERROR "Cannot find ${file} in ${prefix}."
                                "Cannot continue.")
        endif()
    endforeach()
    # If we are here, then all is well. Return the outs.
    set(${outIncludeDirs} "${prefix}/include" PARENT_SCOPE)
    set(${outLinkDirs} "${prefix}/lib" PARENT_SCOPE)
    set(${outLinkLibs} "lldb" PARENT_SCOPE)
endfunction()
