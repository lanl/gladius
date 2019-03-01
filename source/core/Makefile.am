#
# Copyright (c) 2014-2016 Triad National Security, LLC
#                         All rights reserved.
#
# This file is part of the Gladius project. See the LICENSE.txt file at the
# top-level directory of this distribution.
#

# See: LT-VERSION-SCHEME.txt For Guidance
LIBVINFO = "1:0:0"

lib_LTLIBRARIES = \
libGladiusCore.la

include_HEADERS = \
gladius-rc.h

################################################################################
################################################################################
libGladiusCore_la_SOURCES = \
core.h \
gladius-rc.h \
args.h \
process-landscape.h \
colors.h colors.cpp \
console.h \
env.h env.cpp \
exception.h exception.cpp \
macros.h \
session.h session.cpp \
base64.h base64.c \
utils.h utils.cpp

libGladiusCore_la_CFLAGS =

libGladiusCore_la_CPPFLAGS = \
-I${top_srcdir}/source

libGladiusCore_la_LDFLAGS = -version-info ${LIBVINFO}

libGladiusCore_la_LIBADD =
