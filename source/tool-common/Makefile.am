#
# Copyright (c) 2014-2016 Los Alamos National Security, LLC
#                         All rights reserved.
#
# This file is part of the Gladius project. See the LICENSE.txt file at the
# top-level directory of this distribution.
#

LIBVINFO = "1:0:0"

lib_LTLIBRARIES = \
libGladiusToolCommon.la

include_HEADERS = \
gladius-tli.h

libGladiusToolCommon_la_SOURCES = \
session-key.h \
gladius-tli.h \
faux-mpir.h \
tool-common.h tool-common.cpp

libGladiusToolCommon_la_CFLAGS =

libGladiusToolCommon_la_CPPFLAGS = \
-I${top_srcdir}/source \
${GLADIUS_TOOL_COMMON_CPPFLAGS}

libGladiusToolCommon_la_LDFLAGS = \
-version-info ${LIBVINFO}

libGladiusToolCommon_la_LIBADD = \
${top_builddir}/source/core/libGladiusCore.la
