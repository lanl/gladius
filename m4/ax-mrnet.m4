#
# Copyright (c) 2015      Los Alamos National Security, LLC
#                         All rights reserved.
#
# This FILE is part of the Gladius project. See the LICENSE.txt FILE at the
# top-level directory of this distribution.
#
# SYNOPSIS
#
#   AX_MRNET
#
# DESCRIPTION
#
#   Test for MRNet headers and libraries.
#
#   This macro calls:
#
#     AC_SUBST(MRNET_CPPFLAGS)
#     AC_SUBST(MRNETFE_LDFLAGS)
#     AC_SUBST(MRNETBE_LDFLAGS)
#
#   And sets:
#
#     HAVE_MRNET
#

AC_DEFUN([AX_MRNET], [
dnl The --with-mrnet option
AC_ARG_WITH([mrnet],
    [AS_HELP_STRING(
         [--with-mrnet=ARG],
         [use MRNet from the specified location (Required ARG=<path>)]
     )
    ],
    [ax_mrnet_path=""
     if test "x$withval" = "xno"; then
         want_mrnet="no"
     elif test "x$withval" = "xyes"; then
         want_mrnet="yes"
     else
         want_mrnet="yes"
         ax_mrnet_path="$withval"
     fi
    ],
    [ax_mrnet_path=""
     want_mrnet="yes"
    ]
)

if test "x$ax_mrnet_path" = "x"; then
    AC_MSG_ERROR([Please specify a path to your MRNet installation.])
else
    succeeded=no
    includesubdirs="include"
    libsubdirs="lib"
    # The MRNet front-end libraries.
    fe_link_libs="-lmrnet -lxplat"
    # The MRNet back-end libraries.
    be_link_libs=""
    MRNET_CPPFLAGS="-I$ax_mrnet_path/$includesubdirs"
    MRNET_CPPFLAGS="$MRNET_CPPFLAGS -I$ax_mrnet_path/lib/xplat-4.0.0/include"
    MRNETFE_LDFLAGS="-L$ax_mrnet_path/$libsubdirs $fe_link_libs"
    MRNETBE_LDFLAGS="-L$ax_mrnet_path/$libsubdirs $be_link_libs"

    CPPFLAGS_SAVED="$CPPFLAGS"
    CPPFLAGS="$CPPFLAGS $MRNET_CPPFLAGS"
    export CPPFLAGS

    LDFLAGS_SAVED="$LDFLAGS"
    LDFLAGS="$LDFLAGS $MRNETFE_LDFLAGS"
    export LDFLAGS

    AC_REQUIRE([AC_PROG_CXX])
    AC_LANG_PUSH(C++)
    AC_LINK_IFELSE(
        [AC_LANG_PROGRAM([[@%:@include "mrnet/MRNet.h"]],
                         [[MRN::Network *net;]]
         )
        ],
        [succeeded=yes],
        []
    )
    AC_LANG_POP([C++])
    if test "x$succeeded" = "xno"; then
        AC_MSG_ERROR(
            [Cannot Compile/Link MRNet Apps with Current Setup.]
        )
    fi
    AC_SUBST(MRNET_CPPFLAGS)
    AC_SUBST(MRNETFE_LDFLAGS)
    AC_SUBST(MRNETBE_LDFLAGS)
    AC_DEFINE(HAVE_MRNET,,[define if the MRNet library is available])
    # Cleanup
    CPPFLAGS="$CPPFLAGS_SAVED"
    LDFLAGS="$LDFLAGS_SAVED"
fi
])dnl
