/**
 * Copyright (c) 2015      Los Alamos National Security, LLC
 *                         All rights reserved.
 *
 * This file is part of the Gladius project. See the LICENSE.txt file at the
 * top-level directory of this distribution.
 */

#ifndef TIMELINE_MAIN_WINDOW_H_INCLUDED
#define TIMELINE_MAIN_WINDOW_H_INCLUDED

#include <QWidget>

QT_BEGIN_NAMESPACE
class QGraphicsScene;
QT_END_NAMESPACE

class MainWindow : public QWidget {
    Q_OBJECT
public:
    MainWindow(QWidget *parent = nullptr);

private:
    QGraphicsScene *mScene = nullptr;
};

#endif // TIMELINE_MAIN_WINDOW_H_INCLUDED
