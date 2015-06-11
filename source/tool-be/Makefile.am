#
# Copyright (c) 2014-2015 Los Alamos National Security, LLC
#                         All rights reserved.
#
# This file is part of the Gladius project. See the LICENSE.txt file at the
# top-level directory of this distribution.
#

bin_PROGRAMS = \
gladius-toold

gladius_toold_SOURCES = \
gladius-toold.cpp \
tool-be.h tool-be.cpp

gladius_toold_CFLAGS =

gladius_toold_CPPFLAGS = \
-I${top_srcdir}/source \
${GLADIUS_TOOL_COMMON_CPPFLAGS} \
${LMON_CPPFLAGS} \
${MRNET_CPPFLAGS}

gladius_toold_LDFLAGS =

gladius_toold_LDADD = \
${top_builddir}/source/core/libGladiusCore.la \
${top_builddir}/source/dspa/core/libGladiusDSPManager.la \
${top_builddir}/source/tool-common/libGladiusToolCommon.la \
${top_builddir}/source/dmi/libGladiusDMI.la \
${top_builddir}/source/lmon/libGladiusLaunchMONBE.la \
${top_builddir}/source/mrnet/libGladiusMRNetBE.la
