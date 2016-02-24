#
# SYNOPSIS
#
#   AX_PROGS_MPICOMPS()
#
# DESCRIPTION
# checks for MPI wrapper compiler support.
#
# COPYRIGHT
# Copyright (c) 2013-2016 Los Alamos National Security, LLC.
#                         All rights reserved.
#


AC_DEFUN([AX_PROGS_MPICOMPS], [dnl
    dnl Publish MPICXX environment variable
    AC_ARG_VAR([MPICXX], [MPI C++ compiler wrapper command (default CXX)])

    dnl check for MPI CXX support
    AC_LANG_PUSH([C++])
    CXX_SAV="$CXX"
    AC_MSG_CHECKING([MPICXX set])
    if test "x$MPICXX" = "x"; then
        AC_MSG_RESULT([no])
    else
        CXX="$MPICXX"
        AC_MSG_RESULT([yes])
    fi
    dnl First see if this is a C++ compiler
    CAN_CXX=no
    AC_MSG_CHECKING([MPICXX can compile/link C++])
    AC_LINK_IFELSE(
        [AC_LANG_PROGRAM([#include <iostream>], [std::cout << 1;])],
        [CAN_CXX=yes],
        [CAN_CXX=no]
    )
    AX_PROGS_MPICOMPS_HAVE_MPICXX=0
    if test "$CAN_CXX" = "yes"; then
        AC_MSG_RESULT([yes])
        AC_CHECK_FUNC([MPI_Init],
                      [AX_PROGS_MPICOMPS_HAVE_MPICXX=1], [])
    else
        AC_MSG_RESULT([no])
    fi
    CXX="$CXX_SAV"
    AC_LANG_POP([C++])
])
