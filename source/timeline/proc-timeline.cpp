/**
 * Copyright (c) 2015      Los Alamos National Security, LLC
 *                         All rights reserved.
 *
 * This file is part of the Gladius project. See the LICENSE.txt file at the
 * top-level directory of this distribution.
 */

#include "common.h"
#include "proc-timeline.h"

#include <QBrush>
#include <QDebug>
#include <QPainter>
#include <QGraphicsView>

#include <iostream>

ProcTimeline::ProcTimeline(
    ProcType procType,
    QGraphicsView *parent
) : mProcType(procType)
  , mView(parent)
{
    // Add an empty line to the scene. The line will be updated when needed.
    mTimeAxisLine = mView->scene()->addLine(QLineF());
}

QRectF
ProcTimeline::boundingRect(void) const
{
    return QRectF();
}

void
ProcTimeline::addTask(
    const TaskInfo &info
) {
    using namespace boost::icl;

    const ustime_t startTime = info.uStartTime;
    const ustime_t stopTime  = info.uStopTime;

    auto closedInterval = construct< discrete_interval<ustime_t> >(
        startTime, stopTime, interval_bounds::closed()
    );
    //
    mTimeIntervalMap.add(std::make_pair(closedInterval, 1));
    //
    auto itRes = mTimeIntervalMap.equal_range(closedInterval);
    for (auto it = itRes.first; it != itRes.second; ++it) {
        std::cerr << "== " << it->first << " " << it->second << std::endl;
    }
    std::cerr << std::endl;
    //
    const qreal x = qreal(startTime / sMicroSecPerPixel);
    const qreal y = pos().y();
    //
    TaskWidget *taskWidget = new TaskWidget(info);
    if (!mColorPalette.empty()) {
        taskWidget->setFillColor(mColorPalette[info.funcID]);
    }
    taskWidget->setPos(x, y);
    mTaskWidgets << taskWidget;
    // Add this now to the scene so we can get an updated scene width for
    // the line drawing.
    mView->scene()->addItem(taskWidget);
    // Update x-axis geometry.
    const qreal sceneWidth = scene()->width();
    // The amount of spacing between the task widget and the timeline.
    static const qreal lineWidgetSpacing = 6.0;
    // y1 and y2 will always be the sdame. Add the widget's height because y
    // seems to be coming from the top.
    const qreal xAxisY = y + taskWidget->getHeight() + lineWidgetSpacing;
    //
    //
    mTimeAxisLine->setLine(
        0.0,
        xAxisY,
        sceneWidth,
        xAxisY
    );
}

void
ProcTimeline::paint(
    QPainter * /*painter */,
    const QStyleOptionGraphicsItem * /* option */,
    QWidget * /* widget */
) { }
