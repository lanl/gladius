#
# Copyright (c) 2015      Los Alamos National Security, LLC
#                         All rights reserved.
#
# This FILE is part of the Gladius project. See the LICENSE.txt FILE at the
# top-level directory of this distribution.
#

QT += core concurrent widgets gui

qtHaveModule(printsupport): QT += printsupport
qtHaveModule(opengl): QT += opengl

TEMPLATE = app

TARGET = timeline

INCLUDEPATH += .

# TODO Pull top-level configury and populate with that.
QMAKE_CXXFLAGS += -Wextra -std=c++11

CONFIG += c++11

# Primarily for Boost on OS X (homebrew)
macx {
    QMAKE_CXXFLAGS += -I/usr/local/include
}

# Input
SOURCES += \
main-window.cpp \
legion-prof-log-parser.cpp \
main-frame.cpp \
proc-timeline.cpp \
main.cpp \
graph-widget.cpp

HEADERS += \
common.h \
info-types.h \
main-window.h \
legion-prof-log-parser.h \
main-frame.h \
proc-timeline.h \
graph-widget.h \
color-palette-factory.h

RESOURCES += \
images.qrc \
html.qrc
