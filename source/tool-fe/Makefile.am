#
# Copyright (c) 2014-2016 Los Alamos National Security, LLC
#                         All rights reserved.
#
# This file is part of the Gladius project. See the LICENSE.txt file at the
# top-level directory of this distribution.
#

LIBVINFO = "1:0:0"

lib_LTLIBRARIES = \
libGladiusToolFE.la

libGladiusToolFE_la_SOURCES = \
tool-fe.h tool-fe.cpp

libGladiusToolFE_la_CFLAGS =

libGladiusToolFE_la_CPPFLAGS = \
-I${top_srcdir}/source \
${GLADIUS_TOOL_COMMON_CPPFLAGS} \
${MRNET_CPPFLAGS}

libGladiusToolFE_la_LDFLAGS =

libGladiusToolFE_la_LIBADD = \
${top_builddir}/source/core/libGladiusCore.la \
${top_builddir}/source/dspa/core/libGladiusDSPManager.la \
${top_builddir}/source/tool-common/libGladiusToolCommon.la \
${top_builddir}/source/mrnet/libGladiusMRNetFE.la
