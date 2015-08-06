/**
 * Copyright (c) 2015      Los Alamos National Security, LLC
 *                         All rights reserved.
 *
 * This file is part of the Gladius project. See the LICENSE.txt file at the
 * top-level directory of this distribution.
 */

#include "graph-widget.h"
#include "proc-timeline.h"

namespace {
// enable if you want to use "debug colors."
static const bool gDrawDebugColors = true;
} // end namespace

GraphWidget::GraphWidget(
    QWidget *parent
) : QGraphicsView(parent)
  , mScene(new QGraphicsScene(this))
{
    setOptimizationFlags(QGraphicsView::DontSavePainterState);
    //
    setViewportUpdateMode(QGraphicsView::SmartViewportUpdate);
    //
    setFrameShape(QFrame::NoFrame);
    //
    setDragMode(QGraphicsView::NoDrag);
    //
    setRenderHint(QPainter::Antialiasing, true);
    //
    if (gDrawDebugColors) {
        setBackgroundBrush(QBrush(Qt::gray));
    }
    //
    setScene(mScene);
}

void
GraphWidget::addProcTimeline(
    ProcType type
) {
    static qreal y = 0;
    ProcTimeline *tl = new ProcTimeline(type, this);
    tl->setY(y);
    y += 32;
    mProcTimelines << tl;
    mScene->addItem(tl);
}

void
GraphWidget::plot(
    const LegionProfData &plotData
) {
    // Create the proc timelines.
    for (const auto &procDesc : plotData.procDescs) {
        addProcTimeline(procDesc.kind);
    }
    // Populate them...
    for (const auto &taskInfo : plotData.taskInfos) {
        mProcTimelines.at(taskInfo.procID)->addTask(taskInfo);
    }
}
