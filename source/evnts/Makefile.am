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
libevnts.la

include_HEADERS = \
evnts.h

libevnts_la_SOURCES = \
evnts.c evnts.h

libevnts_la_CFLAGS =

libevnts_la_CPPFLAGS = \
-I${top_srcdir}/source

libevnts_la_LDFLAGS = -version-info ${LIBVINFO}

libevnts_la_LIBADD =
