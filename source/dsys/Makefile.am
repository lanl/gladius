#
# Copyright (c) 2016      Los Alamos National Security, LLC
#                         All rights reserved.
#
# This file is part of the Gladius project. See the LICENSE.txt file at the
# top-level directory of this distribution.
#

# See: LT-VERSION-SCHEME.txt For Guidance
LIBVINFO = "1:0:0"

lib_LTLIBRARIES = \
libGladiusDSI.la

libGladiusDSI_la_SOURCES = \
cmdr.h \
dsi.h dsi.cpp

libGladiusDSI_la_CFLAGS =

libGladiusDSI_la_CPPFLAGS = \
-I${top_srcdir}/source

libGladiusDSI_la_LDFLAGS = \
-version-info ${LIBVINFO}

libGladiusDSI_la_LIBADD = \
${top_builddir}/source/core/libGladiusCore.la
