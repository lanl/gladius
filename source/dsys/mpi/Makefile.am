#
# Copyright (c)      2016 Los Alamos National Security, LLC
#                         All rights reserved.
#
# This file is part of the Gladius project. See the LICENSE.txt file at the
# top-level directory of this distribution.
#

# This is an MPI application, so use MPI's compiler wrapper
CXX = ${MPICXX}

bin_PROGRAMS = \
gladius-dsys

gladius_dsys_SOURCES = \
dsys.cpp

gladius_dsys_CPPFLAGS = \
-I${top_srcdir}/source

gladius_dsys_LDADD = \
${top_builddir}/source/core/libGladiusCore.la
