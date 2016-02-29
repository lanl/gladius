#
# Copyright (c) 2014-2016 Los Alamos National Security, LLC
#                         All rights reserved.
#
# This file is part of the Gladius project. See the LICENSE.txt file at the
# top-level directory of this distribution.
#

bin_PROGRAMS = \
gladius

gladius_SOURCES = \
gladius.h gladius.cpp \
gladius-main.cpp

gladius_CFLAGS =

gladius_CPPFLAGS = \
-I${top_srcdir}/source \
${GLADIUS_TOOL_COMMON_CPPFLAGS} \
${MRNET_CPPFLAGS}

gladius_LDADD = \
${top_builddir}/source/core/libGladiusCore.la \
${top_builddir}/source/dspa/core/libGladiusDSPManager.la \
${top_builddir}/source/tool-common/libGladiusToolCommon.la \
${top_builddir}/source/dsys/libGladiusDSI.la \
${top_builddir}/source/mrnet/libGladiusMRNetFE.la \
${top_builddir}/source/tool-fe/libGladiusToolFE.la \
${top_builddir}/source/ui/libGladiusUI.la \
${top_builddir}/source/ui/term/libGladiusUITerm.la

gladius_LDFLAGS =

gladius_LIBS =
