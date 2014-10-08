#
# Copyright (c) 2014      Los Alamos National Security, LLC
#                         All rights reserved.
#
# This file is part of the Gladius project. See the LICENSE.txt file at the
# top-level directory of this distribution.
#

# shows final configuration 
function(echoFinalSetup)
    message("== Final Gladius Configuration")
    message("-- Build Type: ${GLADIUS_BUILD_TYPE_STRING}")
    message("-- CMAKE_CXX_FLAGS: ${CMAKE_CXX_FLAGS}")
endfunction()
