#
# Copyright (c) 2014-2015 Los Alamos National Security, LLC
#                         All rights reserved.
#
# This file is part of the Gladius project. See the LICENSE.txt file at the
# top-level directory of this distribution.
#

function(FIND_THREADS OUT_CFLAGS OUT_LINK_LIBS)
    set(CMAKE_THREAD_PREFER_PTHREAD TRUE)
    find_package(Threads REQUIRED)
    if(CMAKE_USE_PTHREADS_INIT)
        set(CMAKE_C_FLAGS ${CMAKE_C_FLAGS} "-pthread")
    elseif()
        message(FATAL_ERROR "Need pthread support, but not found.")
    endif()
    # If we are here, then all is well. Return the outs.
    set(${OUT_CFLAGS} "-pthread" PARENT_SCOPE)
    set(${OUT_LINK_LIBS} ${CMAKE_THREAD_LIBS_INIT} PARENT_SCOPE)
endfunction()
