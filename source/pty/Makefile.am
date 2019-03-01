#
# Copyright (c) 2015      Triad National Security, LLC
#                         All rights reserved.
#
# This file is part of the Gladius project. See the LICENSE.txt file at the
# top-level directory of this distribution.
#

# See: LT-VERSION-SCHEME.txt For Guidance
LIBVINFO = "1:0:0"

lib_LTLIBRARIES = \
libGladiusDMI.la

libGladiusDMI_la_SOURCES = \
dmi.h dmi.cpp

libGladiusDMI_la_CFLAGS =

libGladiusDMI_la_CPPFLAGS = \
-I${top_srcdir}/source

libGladiusDMI_la_LDFLAGS = \
-version-info ${LIBVINFO}

libGladiusDMI_la_LIBADD = \
${top_builddir}/source/core/libGladiusCore.la
