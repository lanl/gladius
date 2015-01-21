#
# Copyright (c) 2014-2015 Los Alamos National Security, LLC
#                         All rights reserved.
#
# This file is part of the Gladius project. See the LICENSE.txt file at the
# top-level directory of this distribution.
#

LIBVINFO = "1:0:0"

lib_LTLIBRARIES = \
libGladiusUITerm.la

libGladiusUITerm_la_SOURCES = \
term-cmds.h \
term.h term.cpp

libGladiusUITerm_la_CFLAGS =

libGladiusUITerm_la_CPPFLAGS = \
-I${top_srcdir}/source \
${GLADIUS_TOOL_COMMON_CPPFLAGS} \
${LMON_CPPFLAGS} \
${MRNET_CPPFLAGS}

libGladiusUITerm_la_LDFLAGS = \
${LIBEDIT_LIBS} \
-version-info ${LIBVINFO}

libGladiusUITerm_la_LIBADD = \
${top_builddir}/source/core/libGladiusCore.la \
${top_builddir}/source/lmon/libGladiusLaunchMONFE.la \
${top_builddir}/source/mrnet/libGladiusMRNetFE.la \
${top_builddir}/source/ui/libGladiusUI.la
