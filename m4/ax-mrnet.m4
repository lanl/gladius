#
# Copyright (c) 2015-2016 Los Alamos National Security, LLC
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
    AC_MSG_ERROR([Please specify a path to your MRNet installation with --with-mrnet])
else
    includesubdirs="include"
    libsubdirs="lib"
    # The MRNet front-end libraries.
    mrnet_fe_link_libs="-lmrnet -lxplat"
    # The MRNet back-end libraries.
    mrnet_be_link_libs="-lmrnet -lxplat"
    MRNET_CPPFLAGS="-I$ax_mrnet_path/$includesubdirs"
    # deal with changing path names.
    AC_MSG_CHECKING([MRNet xplat version])
    mrnet_lib_xplat_prefix="$ax_mrnet_path/lib"
    xplat_vstr=`ls -1 $mrnet_lib_xplat_prefix | grep xplat-`
    if test $? != 0; then
        AC_MSG_RESULT([quien sabe])
        AC_MSG_ERROR([Could not determine xplat version])
    else
        AC_MSG_RESULT([$xplat_vstr])
    fi
    MRNET_CPPFLAGS="$MRNET_CPPFLAGS -I$mrnet_lib_xplat_prefix/$xplat_vstr/include"
    MRNETFE_LDFLAGS="-L$ax_mrnet_path/$libsubdirs $mrnet_fe_link_libs"
    MRNETBE_LDFLAGS="-L$ax_mrnet_path/$libsubdirs $mrnet_be_link_libs"

    CPPFLAGS_SAVED="$CPPFLAGS"
    CPPFLAGS="$CPPFLAGS $MRNET_CPPFLAGS"
    export CPPFLAGS

    LDFLAGS_SAVED="$LDFLAGS"
    LDFLAGS="$LDFLAGS $MRNETFE_LDFLAGS"
    export LDFLAGS

    LIBS_SAVED="$LIBS"

    AC_REQUIRE([AC_PROG_CXX])
    AC_LANG_PUSH(C++)
    AC_CHECK_HEADERS(
        [mrnet/MRNet.h],
        [],
        [AC_MSG_ERROR([Cannot find MRNet header: mrnet/MRNet.h])]
    )
    AC_LINK_IFELSE(
        [AC_LANG_PROGRAM(#include "mrnet/MRNet.h"
                         MRN::Network *net;)],
        [libmrnet_found=yes],
        [libmrnet_found=no]
    )
    if test "x$libmrnet_found" = "xno"; then
        AC_MSG_ERROR([Cannot link to MRNet.])
    fi
    # Determine key constant values from MRNet headers.
    AC_MSG_CHECKING([MRNet FirstApplicationTag value])
    ac_gladius_mrnet_FirstApplicationTag="0"
    AC_COMPUTE_INT(
        [ac_gladius_mrnet_FirstApplicationTag],
        [FirstApplicationTag],
        [#include "mrnet/Types.h"],
        [AC_MSG_ERROR([Cannot determine value of FirstApplicationTag])]
    )
    AC_DEFINE_UNQUOTED(
        [GLADIUS_MRNET_FIRST_APP_TAG],
        [$ac_gladius_mrnet_FirstApplicationTag],
        [MRNet first application tag.]
    )
    AC_MSG_RESULT([$ac_gladius_mrnet_FirstApplicationTag])
    #
    AC_LANG_POP([C++])
    AC_SUBST(MRNET_CPPFLAGS)
    AC_SUBST(MRNETFE_LDFLAGS)
    AC_SUBST(MRNETBE_LDFLAGS)
    AC_DEFINE(HAVE_MRNET,,[define if the MRNet library is available])
    # Cleanup
    CPPFLAGS="$CPPFLAGS_SAVED"
    LDFLAGS="$LDFLAGS_SAVED"
    LIBS="$LIBS_SAVED"
fi
])dnl
