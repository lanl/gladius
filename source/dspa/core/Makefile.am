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
libGladiusDSPI.la \
libGladiusDSPManager.la

include_HEADERS = \
gladius-dspi.h

libGladiusDSPI_la_SOURCES = \
gladius-dspi.h

libGladiusDSPI_la_CFLAGS =

libGladiusDSPI_la_CPPFLAGS = \
-I${top_srcdir}/source \
${GLADIUS_PLUGIN_CPPFLAGS}

libGladiusDSPI_la_LDFLAGS = \
-version-info ${LIBVINFO}

libGladiusDSPI_la_LIBADD =

################################################################################
libGladiusDSPManager_la_SOURCES = \
dsp-manager.h dsp-manager.cpp

libGladiusDSPManager_la_CFLAGS =

libGladiusDSPManager_la_CPPFLAGS = \
-I${top_srcdir}/source \
${GLADIUS_PLUGIN_CPPFLAGS}

libGladiusDSPManager_la_LDFLAGS = \
-version-info ${LIBVINFO}

libGladiusDSPManager_la_LIBADD = \
${top_builddir}/source/core/libGladiusCore.la
