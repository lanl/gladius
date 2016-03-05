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
libGladiusToolBEAPI.la

libGladiusToolBEAPI_la_SOURCES = \
toolbe-api.h

libGladiusToolBEAPI_la_CFLAGS =

libGladiusToolBEAPI_la_CPPFLAGS = \
-I${top_srcdir}/source

libGladiusToolBEAPI_la_LDFLAGS = -version-info ${LIBVINFO}

libGladiusToolBEAPI_la_LIBADD = \
${top_builddir}/source/core/libGladiusCore.la \
${top_builddir}/source/tool-common/libGladiusToolCommon.la \
${top_builddir}/source/mrnet/libGladiusMRNetBE.la
