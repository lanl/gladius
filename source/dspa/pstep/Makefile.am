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
psteplibdir = $(libdir)/pstep

# And here, notice the [plugin-name]lib_LTLIBRARIES. MODIFY THIS FOR YOUR NEEDS.
# Don't change the names of the .las. Gladius expects these naming conventions.
psteplib_LTLIBRARIES = \
PluginFrontEnd.la

# Add your tool front-end sources here.
PluginFrontEnd_la_SOURCES = \
pstep-fe.cpp

PluginFrontEnd_la_CFLAGS =

PluginFrontEnd_la_CPPFLAGS = \
-I${top_srcdir}/source \
-I${top_srcdir}/source/dspa/core \
${GLADIUS_PLUGIN_CPPFLAGS}

PluginFrontEnd_la_LDFLAGS = \
-module -avoid-version

PluginFrontEnd_la_LIBADD =
