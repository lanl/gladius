/**
 * Copyright (c) 2015      Los Alamos National Security, LLC
 *                         All rights reserved.
 *
 * This file is part of the Gladius project. See the LICENSE.txt file at the
 * top-level directory of this distribution.
 */

#include "graph-widget.h"
#include "proc-timeline.h"
#include "color-palette-factory.h"

#include <QDebug>
#ifndef QT_NO_OPENGL
#include <QtOpenGL>
#else
#include <QtWidgets>
#endif

#include <qmath.h>

namespace {
// enable if you want to use "debug colors."
static const bool gDrawDebugColors = false;
} // end namespace

GraphWidget::GraphWidget(
    QWidget *parent
) : QGraphicsView(parent)
  , mScene(new QGraphicsScene(this))
{
    setFrameShape(QFrame::NoFrame);
    //
    setViewportUpdateMode(QGraphicsView::SmartViewportUpdate);
    //
    setDragMode(QGraphicsView::RubberBandDrag);
    //
    setRenderHint(QPainter::Antialiasing, true);
    //
#ifndef QT_NO_OPENGL
    // On OS X I get "QMacCGContext:: Unsupported painter devtype type 1"
    // when this is enabled.
    //setViewport(new QGLWidget(QGLFormat(QGL::DoubleBuffer)));
#endif
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
    if (!mProcTimelines.contains(procDesc.procID)) {
        ProcTimeline *tl = new ProcTimeline(procDesc, this);
        mProcTimelines.insert(procDesc.procID, tl);
        mScene->addItem(tl);
        updateProcTimelineLayout();
    }
}

void
GraphWidget::addPlotData(
    const LegionProfData &plotData
) {
    QList<QColor> colorPalette = ColorPaletteFactory::getColorAlphabet2();
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
    for (const auto &metaInfo: plotData.metaInfos) {
        mProcTimelines[metaInfo.procID]->addTask(metaInfo);
    }
    //
#if 0 // For debugging time interval data.
    foreach (ProcTimeline *procTimeline, mProcTimelines) {
        procTimeline->debugDumpTimeIntervalData();
    }
#endif
}

void
GraphWidget::plot(
    void
) {
    // Make things look a bit nicer.
    updateProcTimelineLayout();
    //
    foreach (ProcTimeline *timeline, mProcTimelines) {
        timeline->doneAddingTasks();
    }
}

void
GraphWidget::updateProcTimelineLayout(void)
{
    // Spacing between timelines.
    static const qreal spacing = 10.0;
    qreal y = 0.0;
    // QMaps are always sorted by key. This is what we want.
    foreach (ProcTimeline *procTimeline, mProcTimelines) {
        procTimeline->setY(y);
        procTimeline->propagatePositionUpdate();
        y += procTimeline->boundingRect().height() + spacing;
    }
}
