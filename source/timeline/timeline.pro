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
QMAKE_CXXFLAGS += -std=c++11

# Input
SOURCES += \
timeline.cpp \
main-window.cpp \
legion-prof-log-parser.cpp \
main-frame.cpp \
scene.cpp

HEADERS += \
info-types.h \
main-window.h \
legion-prof-log-parser.h \
main-frame.h \
scene.h
