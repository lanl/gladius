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
libGladiusAppLauncher.la

libGladiusAppLauncher_la_SOURCES = \
app-launcher.h \
app-launcher-orte.h

libGladiusAppLauncher_la_CFLAGS =

libGladiusAppLauncher_la_CPPFLAGS = \
-I${top_srcdir}/source

libGladiusAppLauncher_la_LDFLAGS = -version-info ${LIBVINFO}

libGladiusAppLauncher_la_LIBADD = \
${top_builddir}/source/core/libGladiusCore.la \
${top_builddir}/source/tool-common/libGladiusToolCommon.la
