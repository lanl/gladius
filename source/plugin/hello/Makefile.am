#
# Copyright (c) 2015-2016 Triad National Security, LLC
#                         All rights reserved.
#
# This file is part of the Gladius project. See the LICENSE.txt file at the
# top-level directory of this distribution.
#
# This is an MPI application, so use MPI's compiler wrapper
CXX = ${MPICXX}

# Notice the name. The first bit is the plugin name. MODIFY THIS FOR YOUR NEEDS.
# The last bit must be "libdir". For plugins packaged with Gladius, please use
# the $(libdir)[plugin-name] convention for its value. External plugins can be
# installed anywhere, but external paths must be included in
# GLADIUS_PLUGIN_PATH.
hellolibdir = $(libdir)/hello
# For relocating binaries that are generated here. Same idea as hellolibdir.
hellobindir = $(libdir)/hello

################################################################################
################################################################################
hellobin_PROGRAMS = \
hello-app

hello_app_SOURCES = \
hello-app.cpp

hello_app_CPPFLAGS = \
-I${top_srcdir}/source

hello_app_LDADD = \
${top_builddir}/source/core/libGladiusCore.la \
${top_builddir}/source/tool-api/libgladius-tool-be.la

################################################################################
# And here, notice the [plugin-name]lib_LTLIBRARIES. MODIFY THIS FOR YOUR NEEDS.
# Don't change the names of the .las. Gladius expects these naming conventions.
################################################################################
hellolib_LTLIBRARIES = \
PluginFrontEnd.la \
PluginBackEnd.la \
PluginFilters.la

#################################################################################
## Add your tool front-end sources here.
#################################################################################
PluginFrontEnd_la_SOURCES = \
hello-fe.cpp hello-common.h

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
hello-be.cpp hello-common.h

PluginBackEnd_la_CFLAGS =

PluginBackEnd_la_CPPFLAGS = \
-I${top_srcdir}/source \
-I${top_srcdir}/source/dspa/core \
${GLADIUS_PLUGIN_CPPFLAGS}

PluginBackEnd_la_LDFLAGS = \
-module -avoid-version

PluginBackEnd_la_LIBADD =

################################################################################
# Add your tool filters here (optional).
################################################################################
PluginFilters_la_SOURCES = \
hello-filters.h hello-filters.cpp

PluginFilters_la_CFLAGS =

PluginFilters_la_CXXFLAGS =

PluginFilters_la_CPPFLAGS = \
${MRNET_CPPFLAGS}

PluginFilters_la_LDFLAGS = \
-module -avoid-version

PluginFilters_la_LIBADD =
