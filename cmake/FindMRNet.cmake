#
# Copyright (c) 2014      Los Alamos National Security, LLC
#                         All rights reserved.
#
# This file is part of the Gladius project. See the LICENSE.txt file at the
# top-level directory of this distribution.
#

set(
    GLADIUS_MRNET_HOME
    "" CACHE FILEPATH
    "The base directory of the MRNet installation that will be used to build
    gladius. (Required)"
)

function(findMRNet outIncludeDirs outLinkDirs outLinkLibs)
    if (GLADIUS_MRNET_HOME STREQUAL "")
        message(FATAL_ERROR "GLADIUS_MRNET_HOME not defined. Cannot continue.")
    endif()
    set(prefix ${GLADIUS_MRNET_HOME})
    message(STATUS "Looking for MRNet in ${prefix}")
    message(STATUS "Making sure ${prefix} is a valid path...")
    if(NOT EXISTS ${prefix})
        message(FATAL_ERROR "${prefix} does not exist. Cannot continue")
    endif()
    message(STATUS "Making sure ${prefix} has some files we need...")
    file(GLOB headersWeNeed
        "${prefix}/include/mrnet/MRNet.h"
    )
    foreach(file ${headersWeNeed})
        message(STATUS "Looking for ${file}")
        if(NOT EXISTS "${file}")
            message(FATAL_ERROR "Cannot find ${file}. Cannot continue.")
        endif()
    endforeach()
    # Add any needed libraries here
    set(libNames
        "mrnet"
        "xplat"
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
    # Okay, now append dl libs to the list of libName. Since this won't be in
    # the search path, we can't add it initially. So, add it now.
    message(STATUS "Adding needed DL link libs")
    list(APPEND libNames "${CMAKE_DL_LIBS}")
    # Same goes for pthread
    message(STATUS "Adding needed thread link libs")
    find_package(Threads REQUIRED)
    list(APPEND libNames "${CMAKE_THREAD_LIBS_INIT}")
    # ... and same with boost
    find_package(Boost 1.54.0 COMPONENTS system timer)
    if(NOT Boost_FOUND)
        message(FATAL_ERROR "Cannot find necessary library: Boost "
                            "Cannot continue."
        )
    endif()
    list(APPEND libNames "${Boost_LIBRARIES}")
    # If we are here, then all is well. Return the outs.
    set(${outIncludeDirs}
        # FIXME add a find for xplat_config.h. Why it's in
        # lib/xplat-4.0.0/include is beyond me...
        "${prefix}/lib/xplat-4.0.0/include"
        "${prefix}/include"
        "(${Boost_INCLUDE_DIRS}"
        PARENT_SCOPE)
    set(${outLinkDirs} "${prefix}/lib" PARENT_SCOPE)
    set(${outLinkLibs} ${libNames} PARENT_SCOPE)
endfunction()
