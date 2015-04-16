#
# Copyright (c) 2015      Los Alamos National Security, LLC
#                         All rights reserved.
#
# This file is part of the Gladius project. See the LICENSE.txt file at the
# top-level directory of this distribution.
#

# Notice the name. The first bit is the plugin name. MODIFY THIS FOR YOUR NEEDS.
# The last bit must be "libdir". For plugins packaged with Gladius, please use
# the $(libdir)[plugin-name] convention for its value. External plugins can be
# installed anywhere, but external paths must be included in
# GLADIUS_PLUGIN_PATH.
pgdblibdir = $(libdir)/pgdb

# And here, notice the [plugin-name]lib_LTLIBRARIES. MODIFY THIS FOR YOUR NEEDS.
# Don't change the names of the .las. Gladius expects these naming conventions.
pgdblib_LTLIBRARIES = \
PluginFrontEnd.la \
PluginBackEnd.la \
PluginFilters.la

################################################################################
# Add your tool front-end sources here.
################################################################################
PluginFrontEnd_la_SOURCES = \
pgdb-fe.cpp pgdb-common.h

PluginFrontEnd_la_CFLAGS =

PluginFrontEnd_la_CPPFLAGS = \
-I${top_srcdir}/source \
-I${top_srcdir}/source/dspa/core \
${GLADIUS_PLUGIN_CPPFLAGS}

PluginFrontEnd_la_LDFLAGS = \
-module -avoid-version

PluginFrontEnd_la_LIBADD =

################################################################################
# Add your tool back-end sources here.
################################################################################
PluginBackEnd_la_SOURCES = \
pgdb-be.cpp pgdb-common.h

PluginBackEnd_la_CFLAGS =

PluginBackEnd_la_CPPFLAGS = \
-I${top_srcdir}/source \
-I${top_srcdir}/source/dspa/core \
${MIGDB_CPPFLAGS} \
${GLADIUS_PLUGIN_CPPFLAGS}

PluginBackEnd_la_LDFLAGS = \
${MIGDB_LDFLAGS} \
-module -avoid-version

PluginBackEnd_la_LIBADD =

################################################################################
# Add your tool filters here (optional).
################################################################################
PluginFilters_la_SOURCES = \
pgdb-filters.h pgdb-filters.cpp

PluginFilters_la_CFLAGS =

PluginFilters_la_CXXFLAGS =

PluginFilters_la_CPPFLAGS = \
${MRNET_CPPFLAGS}

PluginFilters_la_LDFLAGS = \
-module -avoid-version

PluginFilters_la_LIBADD =
