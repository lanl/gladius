#
# Copyright (c) 2014      Los Alamos National Security, LLC
#                         All rights reserved.
#
# This file is part of the Gladius project. See the LICENSE.txt file at the
# top-level directory of this distribution.
#

set(
    GLADIUS_LMON_HOME
    "" CACHE FILEPATH
    "The base directory of the LaunchMON installation that will be used to build
    gladius. (Required)"
)

function(findLMON outIncludeDirs outLinkDirs outLinkLibs)
    if (GLADIUS_LMON_HOME STREQUAL "")
        message(FATAL_ERROR "GLADIUS_LMON_HOME not defined. Cannot continue.")
    endif()
    set(prefix ${GLADIUS_LMON_HOME})
    message(STATUS "Looking for LaunchMON in ${prefix}")
    message(STATUS "Making sure ${prefix} is a valid path...")
    if(NOT EXISTS ${prefix})
        message(FATAL_ERROR "${prefix} does not exist. Cannot continue")
    endif()
    message(STATUS "Making sure ${prefix} has some files we need...")
    file(GLOB headersWeNeed
        "${prefix}/include/lmon_api/lmon_fe.h"
    )
    foreach(file ${headersWeNeed})
        message(STATUS "Looking for ${file}")
        if(NOT EXISTS "${file}")
            message(FATAL_ERROR "Cannot find ${file}. Cannot continue.")
        endif()
    endforeach()
    # Add any needed libraries here
    set(libNames
        "monfeapi"
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
    # Same goes for pthread
    message(STATUS "Adding needed thread link libs")
    find_package(Threads REQUIRED)
    list(APPEND libNames "${CMAKE_THREAD_LIBS_INIT}")
    # If we are here, then all is well. Return the outs.
    set(${outIncludeDirs}
        "${prefix}/include"
        PARENT_SCOPE)
    set(${outLinkDirs} "${prefix}/lib" PARENT_SCOPE)
    set(${outLinkLibs} ${libNames} PARENT_SCOPE)
endfunction()
