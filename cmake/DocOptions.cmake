#
# Copyright (c) 2014      Los Alamos National Security, LLC
#                         All rights reserved.
#
# This file is part of the Gladius project. See the LICENSE.txt file at the
# top-level directory of this distribution.
#

################################################################################
# Top-level call responsible for setting up documentation things. 
################################################################################
function(docSetup)
    # We need Doxygen, so look for it.
    find_package(Doxygen)
    option(BUILD_DOCUMENTATION
           "Create and install the HTML based API documentation (requires Doxygen)"
           ${DOXYGEN_FOUND}
    )
    if(BUILD_DOCUMENTATION)
        if(NOT DOXYGEN_FOUND)
            message(FATAL_ERROR "Doxygen is needed to build the documentation.")
        endif()

        set(doxyfile_in ${CMAKE_CURRENT_SOURCE_DIR}/doc/Doxyfile.in)
        set(doxyfile ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)

        configure_file(${doxyfile_in} ${doxyfile} @ONLY)

        add_custom_target(
            docs
            COMMAND ${DOXYGEN_EXECUTABLE} ${doxyfile}
            WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
            COMMENT "Generating API documentation with Doxygen"
            VERBATIM
        )
        install(DIRECTORY
                ${CMAKE_CURRENT_BINARY_DIR}/html DESTINATION
                share/doc
        )
    endif(BUILD_DOCUMENTATION)
endfunction()
