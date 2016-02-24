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
    dnl Publish MPICC environment variable
    AC_ARG_VAR([MPICC], [MPI C compiler wrapper command (default CC)])

    dnl check for MPI CC support
    CC_SAV="$CC"
    AC_MSG_CHECKING([MPICC set])
    if test "x$MPICC" = "x"; then
        AC_MSG_RESULT([no])
    else
        CC="$MPICC"
        AC_MSG_RESULT([yes])
    fi
    AX_PROGS_MPICOMPS_HAVE_MPICC=0
    AC_CHECK_FUNC([MPI_Init],
                  [AX_PROGS_MPICOMPS_HAVE_MPICC=1], [])
    CC="$CC_SAV"
])
