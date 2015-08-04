/**
 * Copyright (c) 2015      Los Alamos National Security, LLC
 *                         All rights reserved.
 *
 * This file is part of the Gladius project. See the LICENSE.txt file at the
 * top-level directory of this distribution.
 */

#ifndef TIMELINE_GRAPH_WIDGET_H
#define TIMELINE_GRAPH_WIDGET_H

#include "common.h"

#include <QGraphicsView>
#include <QList>

QT_BEGIN_NAMESPACE
class QWidget;
class QGraphicsScene;
QT_END_NAMESPACE

class ProcTimeline;

class GraphWidget : public QGraphicsView {
    Q_OBJECT
public:
    //
    GraphWidget(QWidget *parent = nullptr);
    //
    void addProcTimeline(ProcType type);

private:
    //
    QGraphicsScene *mScene = nullptr;
    //
    QList<ProcTimeline *> mProcTimelines;
};

#endif // TIMELINE_GRAPHWIDGET_H
