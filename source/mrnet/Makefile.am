#
# Copyright (c) 2014-2015 Los Alamos National Security, LLC
#                         All rights reserved.
#
# This file is part of the Gladius project. See the LICENSE.txt file at the
# top-level directory of this distribution.
#

LIBVINFO = "1:0:0"

lib_LTLIBRARIES = \
libGladiusMRNetFE.la \
libGladiusMRNetBE.la

################################################################################
# Front-end
################################################################################
libGladiusMRNetFE_la_SOURCES = \
mrnet-fe.h mrnet-fe.cpp

libGladiusMRNetFE_la_CFLAGS =

libGladiusMRNetFE_la_CXXFLAGS =

libGladiusMRNetFE_la_CPPFLAGS = \
-I${top_srcdir}/source \
${GLADIUS_TOOL_COMMON_CPPFLAGS} \
${MRNET_CPPFLAGS} \
${BOOST_CPPFLAGS}

libGladiusMRNetFE_la_LDFLAGS = \
${MRNETFE_LDFLAGS} \
${BOOST_LDFLAGS} -lboost_timer \
-version-info ${LIBVINFO}

libGladiusMRNetFE_la_LIBADD = \
${top_builddir}/source/core/libGladiusCore.la

################################################################################
# Back-end
################################################################################
libGladiusMRNetBE_la_SOURCES = \
mrnet-be.h mrnet-be.cpp

libGladiusMRNetBE_la_CFLAGS =

libGladiusMRNetBE_la_CXXFLAGS =

libGladiusMRNetBE_la_CPPFLAGS = \
-I${top_srcdir}/source \
${GLADIUS_TOOL_COMMON_CPPFLAGS} \
${MRNET_CPPFLAGS} \
${BOOST_CPPFLAGS}

libGladiusMRNetBE_la_LDFLAGS = \
${MRNETBE_LDFLAGS} -lxplat \
${BOOST_LDFLAGS} -lboost_timer \
-version-info ${LIBVINFO}

libGladiusMRNetBE_la_LIBADD = \
${top_builddir}/source/core/libGladiusCore.la
