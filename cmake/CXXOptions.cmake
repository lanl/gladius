#
# Copyright (c) 2014-2015 Los Alamos National Security, LLC
#                         All rights reserved.
#
# This file is part of the Gladius project. See the LICENSE.txt file at the
# top-level directory of this distribution.
#

################################################################################
# Top-level call that call all CXX flag configury
################################################################################
function(GET_CXX_FLAGS RESULT)
    GET_BASE_GLADIUS_CXX_FLAGS(CXX_FLAGS)
    GET_CXX11_FLAGS(CXX11_FLAGS)
    GET_BUILD_TYPE_CXX_FLAGS(CXX_BUILD_TYPE_FLAGS)
    set(${RESULT}
        "${CXX_FLAGS} ${CXX11_FLAGS} ${CXX_BUILD_TYPE_FLAGS}"
        PARENT_SCOPE
    )
endfunction()

################################################################################
# Returns base CXX flags through OUT_FLAGS
################################################################################
function(GET_BASE_GLADIUS_CXX_FLAGS OUT_FLAGS)
    # start with some basic flags that are common to all build types
    set(${OUT_FLAGS} "-Wall -Wextra" PARENT_SCOPE)
endfunction()

################################################################################
# Returns C++11 Support flags through OUT_FLAGS
################################################################################
include(CheckCXXCompilerFlag)

function(GET_CXX11_FLAGS OUT_FLAGS)
    CHECK_CXX_COMPILER_FLAG("-std=c++11" COMPILER_SUPPORTS_CXX11)
    CHECK_CXX_COMPILER_FLAG("-std=c++0x" COMPILER_SUPPORTS_CXX0X)
    if(COMPILER_SUPPORTS_CXX11)
        set(${OUT_FLAGS} "-std=c++11" PARENT_SCOPE)
    elseif(COMPILER_SUPPORTS_CXX0X)
        set(${OUT_FLAGS} "-std=c++0x" PARENT_SCOPE)
    else()
        message(FATAL_ERROR "The compiler ${CMAKE_CXX_COMPILER} "
                            "has no C++11 support. Please use a different "
                            "C++ compiler."
        )
    endif()
endfunction()

################################################################################
# Returns build-type-specific CXX flags.
################################################################################
function(GET_BUILD_TYPE_CXX_FLAGS OUT_FLAGS)
    if(GLADIUS_BUILD_TYPE_DEBUG)
        set(${OUT_FLAGS} ${CMAKE_CXX_FLAGS_DEBUG} PARENT_SCOPE)
    elseif(GLADIUS_BUILD_TYPE_REL)
        set(${OUT_FLAGS} ${CMAKE_CXX_FLAGS_RELEASE} PARENT_SCOPE)
    elseif(GLADIUS_BUILD_TYPE_REL_WDBI)
        set(${OUT_FLAGS} ${CMAKE_CXX_FLAGS_RELWITHDEBINFO} PARENT_SCOPE)
    endif()
endfunction()
