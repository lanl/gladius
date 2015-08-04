#
# Copyright (c) 2015      Los Alamos National Security, LLC
#                         All rights reserved.
#
# This FILE is part of the Gladius project. See the LICENSE.txt FILE at the
# top-level directory of this distribution.
#

QT += core gui widgets
qtHaveModule(printsupport): QT += printsupport
qtHaveModule(opengl): QT += opengl

TEMPLATE = app

TARGET = timeline

INCLUDEPATH += .

# TODO Pull top-level configury and populate with that.
QMAKE_CXXFLAGS += -Wextra -std=c++11

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
graph-widget.h
