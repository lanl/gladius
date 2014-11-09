#
# Copyright (c) 2014      Los Alamos National Security, LLC
#                         All rights reserved.
#
# This file is part of the Gladius project. See the LICENSE.txt file at the
# top-level directory of this distribution.
#

################################################################################
# Top-level call that call all CXX flag configury
################################################################################
function(getCXXFlags result)
    getBaseGladiusCXXFlags(cxxFlags)
    getCXX11Flags(cxx11Flags)
    getBuildTypeCXXFlags(cxxBuildTypeFlags)
    set(${result}
        "${cxxFlags} ${cxx11Flags} ${cxxBuildTypeFlags}"
        PARENT_SCOPE)
endfunction()

################################################################################
# Returns base CXX flags through outFlags
################################################################################
function(getBaseGladiusCXXFlags outFlags)
    # start with some basic flags that are common to all build types
    set(${outFlags} "-Wall -Wextra" PARENT_SCOPE)
endfunction()

################################################################################
# Returns C++11 Support flags through outFlags
################################################################################
include(CheckCXXCompilerFlag)

function(getCXX11Flags outFlags)
    CHECK_CXX_COMPILER_FLAG("-std=c++11" COMPILER_SUPPORTS_CXX11)
    CHECK_CXX_COMPILER_FLAG("-std=c++0x" COMPILER_SUPPORTS_CXX0X)
    if(COMPILER_SUPPORTS_CXX11)
        set(${outFlags} "-std=c++11" PARENT_SCOPE)
    elseif(COMPILER_SUPPORTS_CXX0X)
        set(${outFlags} "-std=c++0x" PARENT_SCOPE)
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
function(getBuildTypeCXXFlags outFlags)
    if(GLADIUS_BUILD_TYPE_DEBUG)
        set(${outFlags} ${CMAKE_CXX_FLAGS_DEBUG} PARENT_SCOPE)
    elseif(GLADIUS_BUILD_TYPE_REL)
        set(${outFlags} ${CMAKE_CXX_FLAGS_RELEASE} PARENT_SCOPE)
    elseif(GLADIUS_BUILD_TYPE_REL_WDBI)
        set(${outFlags} ${CMAKE_CXX_FLAGS_RELWITHDEBINFO} PARENT_SCOPE)
    endif()
endfunction()
