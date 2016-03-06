#
# Copyright (c) 2015-2016 Los Alamos National Security, LLC
#                         All rights reserved.
#
# This FILE is part of the Gladius project. See the LICENSE.txt FILE at the
# top-level directory of this distribution.
#

AC_PREREQ([2.69])
AC_INIT([gladius], [0.0.1], [samuel@lanl.gov])

AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_SRCDIR([source])

################################################################################
# Automake things.
################################################################################
# NOTE: -Wall -Werror are automake options -- they are NOT compiler flags.
# Run AM_INIT_AUTOMAKE BEFORE AC_PROG_CC and friends.
AM_INIT_AUTOMAKE([1.14 -Wall -Werror foreign])
# Check If AM_SILENT_RULES Rule Are Supported
# If So, Build Using AM_SILENT_RULES
m4_ifdef([AM_SILENT_RULES], [AM_SILENT_RULES([yes])])

################################################################################
# Checks for programs.
################################################################################
AC_PROG_CXX([clang++ g++])
AC_PROG_CC([clang gcc])
AM_PROG_CC_C_O
AC_PROG_CC_C99
AM_PROG_AR
AC_PROG_INSTALL
AC_PROG_MAKE_SET

################################################################################
# MPI compiler wrapper checks
################################################################################
AX_PROGS_MPICOMPS
AS_IF(
    [test "x$AX_PROGS_MPICOMPS_HAVE_MPICXX" != "x1"],dnl
    [AC_MSG_ERROR(
        [MPICXX cannot compile C++ MPI code. Set MPICXX to something that can.]
     )]dnl
)

################################################################################
# Add some nice flags for development.
################################################################################
AC_MSG_CHECKING([if building in developer mode])
if test -d '.git'; then
    AC_MSG_RESULT([yes])
    GLADIUS_DEVEL_BUILD=yes
else
    AC_MSG_RESULT([no])
    GLADIUS_DEVEL_BUILD=no
fi

if test "x${GLADIUS_DEVEL_BUILD}" = "xyes"; then
    case ${CXX} in
        clang++ | g++ ) GLADIUS_DEVEL_CFLAGS="-Wall -Wextra -Wpedantic" ;;
        * )             GLADIUS_DEVEL_CFLAGS="" ;;
    esac
    CFLAGS="${CFLAGS} ${GLADIUS_DEVEL_CFLAGS}"
    CXXFLAGS="${CXXFLAGS} ${GLADIUS_DEVEL_CFLAGS}"
fi

################################################################################
# Checks for libraries.
################################################################################
# To enable, see automake in: https://github.com/bitcoin/bitcoin/pull/5516
# and update .am files accordingly.
#AX_PTHREAD(
#    [],
#    [AC_MSG_ERROR([Pthreads support required, but not found.])]
#)

################################################################################
AX_BOOST_BASE(
    [1.54], [], [AC_MSG_ERROR([Could not find a usable version of boost.])]
)

################################################################################
AC_SEARCH_LIBS(
    [dlopen],
    [dl],
    [],
    [AC_MSG_ERROR([Could not find dlopen.])]
)

################################################################################
# Don't want to modify LIBS, so stash and undo after check.
LIBS_SAVE=${LIBS}
LIBEDIT_LIBS=""
AC_SEARCH_LIBS(
    [el_init],
    [edit],
    [LIBEDIT_LIBS="-ledit"],
    [AC_MSG_ERROR([Could not find libedit.])]
)
AC_SUBST(LIBEDIT_LIBS)
LIBS=${LIBS_SAVE}

################################################################################
AX_MRNET

# Add this for convenience. We want to hide tool-common's CPPFLAGS behind this
# so they can change freely and only be updated in one spot.
GLADIUS_TOOL_COMMON_CPPFLAGS="${MRNET_CPPFLAGS}"
AC_SUBST(GLADIUS_TOOL_COMMON_CPPFLAGS)

################################################################################
# Do a similar things for plugin flags.
GLADIUS_PLUGIN_CPPFLAGS=${GLADIUS_TOOL_COMMON_CPPFLAGS}
AC_SUBST(GLADIUS_PLUGIN_CPPFLAGS)

################################################################################
# Checks for header files.
################################################################################
AC_CHECK_HEADERS([\
fcntl.h inttypes.h limits.h locale.h string.h sys/ioctl.h termios.h unistd.h \
])

################################################################################
# Checks for typedefs, structures, and compiler characteristics.
################################################################################
AX_CXX_COMPILE_STDCXX_11([noext], [mandatory])
AC_CHECK_HEADER_STDBOOL
AC_C_INLINE
AC_TYPE_PID_T
AC_TYPE_SIZE_T
AC_TYPE_SSIZE_T
AC_TYPE_UINT32_T

################################################################################
# Checks for library functions.
################################################################################
AC_FUNC_ERROR_AT_LINE
AC_CHECK_FUNCS([\
atexit gethostname memmove memset mkdir setenv \
setlocale strchr strdup strerror strtol \
])

################################################################################
# Libtool things
################################################################################
LT_PREREQ([2.4.2])
LT_INIT

AC_CONFIG_FILES([
    Makefile
    source/Makefile
    source/core/Makefile
    source/gpa/core/Makefile
    source/tool-common/Makefile
    source/dsys/Makefile
    source/dsys/mpi/Makefile
    source/mrnet/Makefile
    source/mrnet/filters/Makefile
    source/tool-fe/Makefile
    source/tool-be/Makefile
    source/tool-api/Makefile
    source/ui/Makefile
    source/ui/term/Makefile
    source/gladius/Makefile
    source/gpa/hello/Makefile
])

AC_OUTPUT

cat << EOF
+-------------------------------------------------------------------------------
| gladius ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
+-------------------------------------------------------------------------------
| VERSION  : ${VERSION}
| CC       : ${CC}
| CXX      : ${CXX}
| MPICXX   : ${MPICXX}
| CFLAGS   : ${CFLAGS}
| CXXFLAGS : ${CXXFLAGS}
| CPP      : ${CPP}
| CPPFLAGS : ${CPPFLAGS}
| LDFLAGS  : ${LDFLAGS}
| LIBS     : ${LIBS}
| PREFIX   : ${prefix}
+-------------------------------------------------------------------------------
| boost ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
+-------------------------------------------------------------------------------
| BOOST_CPPFLAGS : ${BOOST_CPPFLAGS}
| BOOST_LDFLAGS  : ${BOOST_LDFLAGS}
+-------------------------------------------------------------------------------
| mrnet ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
+-------------------------------------------------------------------------------
| MRNET_HOME : ${ax_mrnet_path}

EOF
