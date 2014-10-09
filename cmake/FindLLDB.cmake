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
    file(GLOB headersWeNeed
        "${prefix}/include/lldb/API/LLDB.h"
    )
    foreach(file ${headersWeNeed})
        message(STATUS "Looking for ${file}")
        if(NOT EXISTS "${file}")
            message(FATAL_ERROR "Cannot find ${file}. Cannot continue.")
        endif()
    endforeach()
    # Add any needed libraries here
    set(libNames
        "lldb"
        "lldbAPI"
    )
    set(libsWeNeed)
    foreach (libName ${libNames})
        message(STATUS "Looking for lib${libName}")
        file(GLOB libWeNeed
            "${prefix}/lib/lib${libName}.*"
        )
        list(APPEND libsWeNeed ${libWeNeed})
        list(LENGTH libWeNeed numFound)
        if(numFound EQUAL 0)
            message(FATAL_ERROR "Cannot find necessary library: lib${libName}. "
                                "Cannot continue."
            )
        endif()
    endforeach()
    # If we are here, then all is well. Return the outs.
    set(${outIncludeDirs} "${prefix}/include" PARENT_SCOPE)
    set(${outLinkDirs} "${prefix}/lib" PARENT_SCOPE)
    set(${outLinkLibs} ${libNames} PARENT_SCOPE)
endfunction()
