#
# Copyright (c) 2014-2016 Los Alamos National Security, LLC
#                         All rights reserved.
#
# This file is part of the Gladius project. See the LICENSE.txt file at the
# top-level directory of this distribution.
#

# See: LT-VERSION-SCHEME.txt For Guidance
LIBVINFO = "1:0:0"

lib_LTLIBRARIES = \
libgladius-tool-be.la

include_HEADERS = \
gladius-toolbe.h

libgladius_tool_be_la_SOURCES =

libgladius_tool_be_la_CFLAGS =

libgladius_tool_be_la_CPPFLAGS = \
-I${top_srcdir}/source

libgladius_tool_be_la_LDFLAGS = -version-info ${LIBVINFO}

libgladius_tool_be_la_LIBADD = \
${top_builddir}/source/core/libGladiusCore.la \
${top_builddir}/source/tool-common/libGladiusToolCommon.la \
${top_builddir}/source/mrnet/libGladiusMRNetBE.la
