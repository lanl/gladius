#
# Copyright (c) 2015-2016 Los Alamos National Security, LLC
#                         All rights reserved.
#
# This file is part of the Gladius project. See the LICENSE.txt file at the
# top-level directory of this distribution.
#

# See: LT-VERSION-SCHEME.txt For Guidance
LIBVINFO = "1:0:0"

lib_LTLIBRARIES = \
libGladiusPluginInterface.la \
libGladiusPluginManager.la

include_HEADERS = \
gpi.h

libGladiusPluginInterface_la_SOURCES = \
gpi.h

libGladiusPluginInterface_la_CFLAGS =

libGladiusPluginInterface_la_CPPFLAGS = \
-I${top_srcdir}/source \
${GLADIUS_PLUGIN_CPPFLAGS}

libGladiusPluginInterface_la_LDFLAGS = \
-version-info ${LIBVINFO}

libGladiusPluginInterface_la_LIBADD =

################################################################################
libGladiusPluginManager_la_SOURCES = \
gp-manager.h gp-manager.cpp

libGladiusPluginManager_la_CFLAGS =

libGladiusPluginManager_la_CPPFLAGS = \
-I${top_srcdir}/source \
${GLADIUS_PLUGIN_CPPFLAGS}

libGladiusPluginManager_la_LDFLAGS = \
-version-info ${LIBVINFO}

libGladiusPluginManager_la_LIBADD = \
${top_builddir}/source/core/libGladiusCore.la
