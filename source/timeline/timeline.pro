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

QMAKE_CXXFLAGS += -std=c++11

# Input
SOURCES += \
timeline.cpp \
main-window.cpp \
view.cpp \
proc-widget.cpp \
timeline-widget.cpp \
    legion-prof-log-parser.cpp

HEADERS += \
info-types.h \
main-window.h \
view.h \
proc-widget.h \
timeline-widget.h \
    legion-prof-log-parser.h
