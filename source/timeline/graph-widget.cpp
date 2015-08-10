/**
 * Copyright (c) 2015      Los Alamos National Security, LLC
 *                         All rights reserved.
 *
 * This file is part of the Gladius project. See the LICENSE.txt file at the
 * top-level directory of this distribution.
 */

#include "graph-widget.h"
#include "proc-timeline.h"

#include <QDebug>
#include <math.h>

namespace {
// enable if you want to use "debug colors."
static const bool gDrawDebugColors = false;
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
    const ProcDesc &procDesc
) {
    static qreal y = 0;
    ProcTimeline *tl = new ProcTimeline(procDesc.kind, this);
    tl->setY(y);
    // FIXME Some timelines are thicker than others.
    y += 70;
    mProcTimelines.insert(procDesc.procID, tl);
    mScene->addItem(tl);
}

// TODO Move into utils
namespace {

// Adapted From: https://wiki.qt.io/Color_palette_generator
QList<QColor>
getColors(uint32_t numColorsNeeded) {
    static const double golden_ratio = 0.618033988749895;
    QList<QColor> brushScale;
    double h = 0;
    uint32_t realNumColorsNeeded = numColorsNeeded + 1;
    for (uint32_t i = 0; i < realNumColorsNeeded; ++i) {
        h = golden_ratio * 360 / realNumColorsNeeded * i;
        h = floor(h * 6);
        brushScale.append(QColor::fromHsv(int(h) % 128, 245, 230, 255));
    }
    qDebug() << "List Size:" << brushScale.size();
    return brushScale;
}

} // end namespace

void
GraphWidget::plot(
    const LegionProfData &plotData
) {
    QList<QColor> colorPalette = getColors(30);
    // Create the proc timelines.
    for (const auto &procDesc : plotData.procDescs) {
        addProcTimeline(procDesc);
    }
    // Set their color palette.
    foreach (ProcTimeline *timeline, mProcTimelines) {
        timeline->setTaskColorPalette(colorPalette);
    }
    // Populate them...
    for (const auto &taskInfo : plotData.taskInfos) {
        mProcTimelines[taskInfo.procID]->addTask(taskInfo);
    }
    //
#if 0
    foreach (ProcTimeline *procTimeline, mProcTimelines) {
        procTimeline->debugDumpTimeIntervalData();
    }
#endif
}
