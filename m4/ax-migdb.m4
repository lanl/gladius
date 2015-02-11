#
# Copyright (c) 2015      Los Alamos National Security, LLC
#                         All rights reserved.
#
# This FILE is part of the Gladius project. See the LICENSE.txt FILE at the
# top-level directory of this distribution.
#
# SYNOPSIS
#
#   AX_MIGDB
#
# DESCRIPTION
#
#   Test for libmigdb headers and libraries.
#
#   This macro calls:
#
#     AC_SUBST(MIGDB_CPPFLAGS)
#     AC_SUBST(MIGDB_LDFLAGS)
#
#   And sets:
#
#     HAVE_MIGDB
#

AC_DEFUN([AX_MIGDB], [
dnl The --with-migdb option
AC_ARG_WITH([migdb],
    [AS_HELP_STRING(
         [--with-migdb=ARG],
         [use libmigdb from the specified location (Required ARG=<path>)]
     )
    ],
    [ax_migdb_path=""
     if test "x$withval" = "xno"; then
         want_migdb="no"
     elif test "x$withval" = "xyes"; then
         want_migdb="yes"
     else
         want_migdb="yes"
         ax_migdb_path="$withval"
     fi
    ],
    [ax_migdb_path=""
     want_migdb="yes"
    ]
)

if test "x$ax_migdb_path" = "x"; then
    AC_MSG_ERROR([Please specify a path to your libmigdb installation with: --with-migdb])
else
    includesubdirs="include"
    libsubdirs="lib"
    # The libmigdb libraries.
    migdb_link_libs="-lmigdb"
    MIGDB_CPPFLAGS="-I$ax_migdb_path/$includesubdirs"
    MIGDB_LDFLAGS="-L$ax_migdb_path/$libsubdirs $migdb_link_libs"

    CPPFLAGS_SAVED="$CPPFLAGS"
    CPPFLAGS="$CPPFLAGS $MIGDB_CPPFLAGS"
    export CPPFLAGS

    LDFLAGS_SAVED="$LDFLAGS"
    LDFLAGS="$LDFLAGS $MIGDB_LDFLAGS"
    export LDFLAGS

    LIBS_SAVED="$LIBS"

    AC_REQUIRE([AC_PROG_CC])
    AC_LANG_PUSH(C)
    AC_CHECK_HEADERS(
        [mi_gdb.h],
        [],
        [AC_MSG_ERROR([Cannot find header: mi_gdb.h])]
    )
    AC_SEARCH_LIBS(
        [mi_connect_local],
        [migdb],
        [],
        [AC_MSG_ERROR([Could not find symbol: mi_connect_local.])]
    )
    # Undo AC_SEARCH_LIBS changes.
    LIBS="$LIBS_SAVED"
    AC_LANG_POP([C])
    AC_SUBST(MIGDB_CPPFLAGS)
    AC_SUBST(MIGDB_LDFLAGS)
    AC_DEFINE(HAVE_MIGDB,,[define if the libmigdb library is available])
    # Cleanup
    CPPFLAGS="$CPPFLAGS_SAVED"
    LDFLAGS="$LDFLAGS_SAVED"
    LIBS="$LIBS_SAVED"
fi
])dnl
