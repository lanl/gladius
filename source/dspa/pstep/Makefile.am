#
# Copyright (c) 2015      Los Alamos National Security, LLC
#                         All rights reserved.
#
# This file is part of the Gladius project. See the LICENSE.txt file at the
# top-level directory of this distribution.
#

# Notice the name. The first bit is the plugin name. MODIFY THIS FOR YOUR NEEDS.
# The last bit must be "libdir". Use the $(libdir)[plugin-name] convention for
# its value.
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
-I${top_srcdir}/source

PluginFrontEnd_la_LDFLAGS = \
-module -avoid-version

PluginFrontEnd_la_LIBADD =
