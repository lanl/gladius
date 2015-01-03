#
# Copyright (c) 2014-2015 Los Alamos National Security, LLC
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

function(FIND_MRNET OUT_INC_DIRS OUT_LINK_DIRS OUT_LINK_LIBS)
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
    file(GLOB HEADERS_WE_NEED
        "${prefix}/include/mrnet/MRNet.h"
    )
    foreach(file ${HEADERS_WE_NEED})
        message(STATUS "Looking for ${file}")
        if(NOT EXISTS "${file}")
            message(FATAL_ERROR "Cannot find ${file}. Cannot continue.")
        endif()
    endforeach()
    # Add any needed libraries here
    set(LIB_NAMES
        "mrnet"
        "xplat"
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
                FATAL_ERROR "Cannot find necessary library: lib${LIB_NAME}. "
                "Cannot continue."
            )
        endif()
    endforeach()
    # Okay, now append dl libs to the list of LIB_NAME. Since this won't be in
    # the search path, we can't add it initially. So, add it now.
    message(STATUS "Adding needed DL link libs")
    list(APPEND LIB_NAMES "${CMAKE_DL_LIBS}")
    # Same goes for pthread
    message(STATUS "Adding needed thread link libs")
    find_package(Threads REQUIRED)
    list(APPEND LIB_NAMES "${CMAKE_THREAD_LIBS_INIT}")
    # ... and same with boost
    find_package(Boost 1.54.0 COMPONENTS system timer)
    if(NOT Boost_FOUND)
        message(FATAL_ERROR "Cannot find necessary library: Boost "
                            "Cannot continue."
        )
    endif()
    list(APPEND LIB_NAMES "${Boost_LIBRARIES}")
    # If we are here, then all is well. Return the outs.
    set(${OUT_INC_DIRS}
        # FIXME add a find for xplat_config.h. Why it's in
        # lib/xplat-4.0.0/include is beyond me...
        "${prefix}/lib/xplat-4.0.0/include"
        "${prefix}/include"
        "(${Boost_INCLUDE_DIRS}"
        PARENT_SCOPE)
    set(${OUT_LINK_DIRS} "${prefix}/lib" PARENT_SCOPE)
    set(${OUT_LINK_LIBS} ${LIB_NAMES} PARENT_SCOPE)
endfunction()
