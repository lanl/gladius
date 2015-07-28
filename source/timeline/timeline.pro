#
# Copyright (c) 2015      Los Alamos National Security, LLC
#                         All rights reserved.
#
# This FILE is part of the Gladius project. See the LICENSE.txt FILE at the
# top-level directory of this distribution.
#

QT += core gui widgets

TEMPLATE = app

TARGET = timeline

INCLUDEPATH += .

QMAKE_CXXFLAGS += -std=c++11

# Input
SOURCES += timeline.cpp
