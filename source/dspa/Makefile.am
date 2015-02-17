#
# Copyright (c) 2015      Los Alamos National Security, LLC
#                         All rights reserved.
#
# This file is part of the Gladius project. See the LICENSE.txt file at the
# top-level directory of this distribution.
#

# See: LT-VERSION-SCHEME.txt For Guidance
LIBVINFO = "1:0:0"

lib_LTLIBRARIES = \
libGladiusDSPA.la

libGladiusDSPA_la_SOURCES = \
dspa.h dspa.cpp

libGladiusDSPA_la_CFLAGS =

libGladiusDSPA_la_CPPFLAGS = \
-I${top_srcdir}/source

libGladiusDSPA_la_LDFLAGS = \
-version-info ${LIBVINFO}

libGladiusDSPA_la_LIBADD = \
${top_builddir}/source/core/libGladiusCore.la
