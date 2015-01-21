#
# Copyright (c) 2015      Los Alamos National Security, LLC
#                         All rights reserved.
#
# This FILE is part of the Gladius project. See the LICENSE.txt FILE at the
# top-level directory of this distribution.
#
# SYNOPSIS
#
#   AX_LMON
#
# DESCRIPTION
#
#   Test for LaunchMON headers and libraries.
#
#   This macro calls:
#
#     AC_SUBST(LMON_CPPFLAGS)
#     AC_SUBST(LMONFE_LDFLAGS)
#     AC_SUBST(LMONBE_LDFLAGS)
#
#   And sets:
#
#     HAVE_LMON
#

AC_DEFUN([AX_LMON], [
dnl The --with-lmon option
AC_ARG_WITH([lmon],
    [AS_HELP_STRING(
         [--with-lmon=ARG],
         [use LaunchMON from the specified location (Required ARG=<path>)]
     )
    ],
    [ax_lmon_path=""
     if test "x$withval" = "xno"; then
         want_lmon="no"
     elif test "x$withval" = "xyes"; then
         want_lmon="yes"
     else
         want_lmon="yes"
         ax_lmon_path="$withval"
     fi
    ],
    [ax_lmon_path=""
     want_lmon="yes"
    ]
)

if test "x$ax_lmon_path" = "x"; then
    AC_MSG_ERROR([Please specify a path to your LaunchMON installation with: --with-lmon])
else
    succeeded=no
    includesubdirs="include"
    libsubdirs="lib"
    # The LaunchMON front-end libraries.
    fe_link_libs="-lmonfeapi"
    # The LaunchMON back-end libraries.
    be_link_libs="-lmonbeapi"
    LMON_CPPFLAGS="-I$ax_lmon_path/$includesubdirs"
    LMONFE_LDFLAGS="-L$ax_lmon_path/$libsubdirs $fe_link_libs"
    LMONBE_LDFLAGS="-L$ax_lmon_path/$libsubdirs $be_link_libs"

    CPPFLAGS_SAVED="$CPPFLAGS"
    CPPFLAGS="$CPPFLAGS $LMON_CPPFLAGS"
    export CPPFLAGS

    LDFLAGS_SAVED="$LDFLAGS"
    LDFLAGS="$LDFLAGS $LMONFE_LDFLAGS"
    export LDFLAGS

    AC_REQUIRE([AC_PROG_CXX])
    AC_LANG_PUSH(C++)
    AC_LINK_IFELSE(
        [AC_LANG_PROGRAM([[@%:@include "lmon_api/lmon_fe.h"]],
                         [[(void)LMON_fe_init(LMON_VERSION);]]
         )
        ],
        [succeeded=yes],
        []
    )
    AC_LANG_POP([C++])
    if test "x$succeeded" = "xno"; then
        AC_MSG_ERROR(
            [Cannot Compile/Link LaunchMON Apps with Current Setup.]
        )
    fi
    AC_SUBST(LMON_CPPFLAGS)
    AC_SUBST(LMONFE_LDFLAGS)
    AC_SUBST(LMONBE_LDFLAGS)
    AC_DEFINE(HAVE_LMON,,[define if the LaunchMON library is available])
    # Cleanup
    CPPFLAGS="$CPPFLAGS_SAVED"
    LDFLAGS="$LDFLAGS_SAVED"
fi
])dnl
