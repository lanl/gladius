#
# Copyright (c) 2014-2016 Los Alamos National Security, LLC
#                         All rights reserved.
#
# This file is part of the Gladius project. See the LICENSE.txt file at the
# top-level directory of this distribution.
#

LIBVINFO = "1:0:0"

lib_LTLIBRARIES = \
libGladiusToolBE.la

libGladiusToolBE_la_SOURCES = \
tool-be.h tool-be.cpp

libGladiusToolBE_la_CFLAGS =

libGladiusToolBE_la_CPPFLAGS = \
-I${top_srcdir}/source \
-I${top_srcdir}/source/core \
${GLADIUS_TOOL_COMMON_CPPFLAGS} \
${MRNET_CPPFLAGS}

libGladiusToolBE_la_LDFLAGS =

libGladiusToolBE_la_LIBADD = \
${top_builddir}/source/core/libGladiusCore.la \
${top_builddir}/source/tool-common/libGladiusToolCommon.la
