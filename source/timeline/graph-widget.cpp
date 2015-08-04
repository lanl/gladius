/**
 * Copyright (c) 2015      Los Alamos National Security, LLC
 *                         All rights reserved.
 *
 * This file is part of the Gladius project. See the LICENSE.txt file at the
 * top-level directory of this distribution.
 */

#include "graph-widget.h"
#include "proc-timeline.h"

GraphWidget::GraphWidget(
    QWidget *parent
) : QGraphicsView(parent)
  , mScene(new QGraphicsScene(this))
{
    setFrameShape(QFrame::NoFrame);
    //
    setDragMode(QGraphicsView::NoDrag);
    //
    setRenderHint(QPainter::Antialiasing, true);
    //
    setOptimizationFlags(QGraphicsView::DontSavePainterState);
    //
    setViewportUpdateMode(QGraphicsView::SmartViewportUpdate);
    // SKG For Debug Views Only
    setBackgroundBrush(QBrush(Qt::gray));
    //
    setScene(mScene);
}

void
GraphWidget::addProcTimeline(
    ProcType type
) {
    static qreal y = 0;
    ProcTimeline *tl = new ProcTimeline(this);
    tl->setY(y);
    y += 64.0;
    mProcTimelines << tl;
    mScene->addItem(tl);
}
