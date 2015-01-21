#
# Copyright (c) 2014-2015 Los Alamos National Security, LLC
#                         All rights reserved.
#
# This file is part of the Gladius project. See the LICENSE.txt file at the
# top-level directory of this distribution.
#

LIBVINFO = "1:0:0"

lib_LTLIBRARIES = \
libGladiusLaunchMONFE.la

libGladiusLaunchMONFE_la_SOURCES = \
lmon-fe.h lmon-fe.cpp

libGladiusLaunchMONFE_la_CFLAGS =

libGladiusLaunchMONFE_la_CPPFLAGS = \
-I${top_srcdir}/source \
${LMON_CPPFLAGS}

libGladiusLaunchMONFE_la_LDFLAGS = \
${LMONFE_LDFLAGS} \
-version-info ${LIBVINFO}

libGladiusLaunchMONFE_la_LIBADD = \
${top_builddir}/source/core/libGladiusCore.la
