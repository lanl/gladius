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
  , mCurrentMaxTaskLevel(1)
{
    mXMax = 128;
    // Add an empty line to the scene. The line will be updated when needed.
    mTimeAxisLine = mView->scene()->addLine(QLineF());
}

QRectF
ProcTimeline::boundingRect(void) const
{
    if (mTaskWidgets.empty()) return QRectF(0, 0, 128, TaskWidget::getHeight());
    return QRectF(0, 0, mXMax, mCurrentMaxTaskLevel * TaskWidget::getHeight());
}

void
ProcTimeline::addTask(
    const TaskInfo &info
) {
    using namespace boost::icl;

    const ustime_t startTime = info.uStartTime / sMicroSecPerPixel;
    const ustime_t stopTime  = info.uStopTime / sMicroSecPerPixel;
    // Stash this so we know when to call updateProcTimelineLayout.
    const auto oldMaxTaskLevel = mCurrentMaxTaskLevel;
    //
    auto closedInterval = construct< discrete_interval<ustime_t> >(
        startTime, stopTime, interval_bounds::closed()
    );
    // The 1 here is the increment when there is an overlap.
    mTimeIntervalMap.add(std::make_pair(closedInterval, 1));
    // Now figure out at what level the task will be drawn. We do this be first
    // determining whether or not there exists overlapping ranges. If so,
    // now determine the max number of overlaps.
    auto itRes = mTimeIntervalMap.equal_range(closedInterval);
    bool overlaps = false;
    if (itRes.first != itRes.second) {
        overlaps = true;
        // it->first = Time Interval.
        // it->second = Number of overlaps in the interval.
        for (auto it = itRes.first; it != itRes.second; ++it) {
            if (it->second > mCurrentMaxTaskLevel) ++mCurrentMaxTaskLevel;
        }
    }
    //
    const qreal x = boundingRect().right();
    const qreal y = pos().y();
    auto minTaskLevel = overlaps ? mCurrentMaxTaskLevel : sMinTaskLevel;
    //
    TaskWidget *taskWidget = new TaskWidget(info, minTaskLevel);
    if (!mColorPalette.empty()) {
        taskWidget->setFillColor(mColorPalette[info.funcID]);
    }
    const qreal taskRight = stopTime;
    prepareGeometryChange();
    if (taskRight > mXMax) mXMax = taskRight;
    update();
    //
    //
    //taskWidget->setPos(x, y + (TaskWidget::getHeight() * minTaskLevel));
    //taskWidget->setPos(qreal(startTime), y);
    mTaskWidgets << taskWidget;
    // Add this now to the scene so we can get an updated scene width for
    // the line drawing.
    //mView->scene()->addItem(taskWidget);
    if (oldMaxTaskLevel != mCurrentMaxTaskLevel) {
        mGraphWidget()->updateProcTimelineLayout();
    }
#if 0
    // Update x-axis geometry.
    const qreal sceneWidth = scene()->width();
    // The amount of spacing between the task widget and the timeline.
    static const qreal lineWidgetSpacing = 6.0;
    // y1 and y2 will always be the sdame. Add the widget's height because y
    // seems to be coming from the top.
    const qreal xAxisY = y + taskWidget->getHeight() + lineWidgetSpacing;
    //
    mTimeAxisLine->setLine(
        0.0,
        xAxisY,
        sceneWidth,
        xAxisY
    );
#endif
}

void
ProcTimeline::paint(
    QPainter * painter,
    const QStyleOptionGraphicsItem * option,
    QWidget * widget
) {
    foreach (TaskWidget *tw, mTaskWidgets) {
        tw->paint(painter, option, widget);
    }
}


