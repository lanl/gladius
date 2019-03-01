/**
 * Copyright (c) 2015      Triad National Security, LLC
 *                         All rights reserved.
 *
 * This file is part of the Gladius project. See the LICENSE.txt file at the
 * top-level directory of this distribution.
 */

#ifndef TIMELINE_MAIN_WINDOW_H_INCLUDED
#define TIMELINE_MAIN_WINDOW_H_INCLUDED

#include <QWidget>

class MainWindow : public QWidget {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
};

#endif // TIMELINE_MAIN_WINDOW_H_INCLUDED
