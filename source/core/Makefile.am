#
# Copyright (c) 2014-2015 Los Alamos National Security, LLC
#                         All rights reserved.
#
# This file is part of the Gladius project. See the LICENSE.txt file at the
# top-level directory of this distribution.
#

# See: LT-VERSION-SCHEME.txt For Guidance
LIBVINFO = "1:0:0"

lib_LTLIBRARIES = \
libGladiusCore.la

libGladiusCore_la_SOURCES = \
core.h \
ret-codes.h \
args.h \
colors.h colors.cpp \
env.h env.cpp \
gladius-exception.h gladius-exception.cpp \
macros.h \
session.h session.cpp \
utils.h utils.cpp

libGladiusCore_la_CFLAGS =

libGladiusCore_la_CPPFLAGS = \
-I${top_srcdir}/source

libGladiusCore_la_LDFLAGS = -version-info ${LIBVINFO}

libGladiusCore_la_LIBADD =
