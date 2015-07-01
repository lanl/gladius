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
libpbnj.la

include_HEADERS = \
pbnj.h

libpbnj_la_SOURCES = \
pbnj.c pbnj.h

libpbnj_la_CFLAGS =

libpbnj_la_CPPFLAGS = \
-I${top_srcdir}/source

libpbnj_la_LDFLAGS = -version-info ${LIBVINFO}

libpbnj_la_LIBADD =
