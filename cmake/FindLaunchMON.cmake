#
# Copyright (c) 2014-2015 Los Alamos National Security, LLC
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

function(FIND_LMON OUT_INC_DIRS OUT_LINK_DIRS OUT_LINK_LIBS_FE OUT_LINK_LIBS_BE)
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
    file(GLOB HEADERS_WE_NEED
        "${prefix}/include/lmon_api/lmon_fe.h"
    )
    foreach(file ${HEADERS_WE_NEED})
        message(STATUS "Looking for ${file}")
        if(NOT EXISTS "${file}")
            message(FATAL_ERROR "Cannot find ${file}. Cannot continue.")
        endif()
    endforeach()
    # Add any needed libraries here. In this case, just check for the front-end
    # libs and assume that the back-end libs are there...
    set(LIB_NAMES_FE
        "monfeapi"
    )
    set(LIB_NAMES_BE
        "monbeapi"
    )
    set(LIBS_WE_NEED)
    foreach (LIB_NAME ${LIB_NAMES_FE})
        message(STATUS "Looking for lib${LIB_NAME}")
        file(GLOB LIB_WE_NEED
            "${prefix}/lib/lib${LIB_NAME}.*"
        )
        list(APPEND LIBS_WE_NEED ${LIB_WE_NEED})
        list(LENGTH LIB_WE_NEED numFound)
        if(numFound EQUAL 0)
            message(FATAL_ERROR "Cannot find necessary library: lib${LIB_NAME}. "
                                "Cannot continue."
            )
        endif()
    endforeach()
    # If we are here, then all is well. Return the outs.
    set(${OUT_INC_DIRS}
        "${prefix}/include"
        PARENT_SCOPE
    )
    set(${OUT_LINK_DIRS} "${prefix}/lib" PARENT_SCOPE)
    set(${OUT_LINK_LIBS_FE} "${LIB_NAMES_FE}" PARENT_SCOPE)
    set(${OUT_LINK_LIBS_BE} "${LIB_NAMES_BE}" PARENT_SCOPE)
endfunction()
