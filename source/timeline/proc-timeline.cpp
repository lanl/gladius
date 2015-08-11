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
  , mCurrentMaxTaskLevel(1) { }

QRectF
ProcTimeline::boundingRect(void) const
{
    if (mTaskWidgets.empty()) return QRectF();
    return QRectF(
        0,
        0,
        mMaxX,
        mCurrentMaxTaskLevel * TaskWidget::getHeight()
    );
}

void
ProcTimeline::addTask(
    const TaskInfo &info
) {
    using namespace boost::icl;

    const ustime_t startTime = info.uStartTime / sMicroSecPerPixel;
    const ustime_t stopTime  = info.uStopTime / sMicroSecPerPixel;
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
    // Stash this so we know when to call updateProcTimelineLayout.
    const auto oldMaxTaskLevel = mCurrentMaxTaskLevel;
    auto thisTaskMinLevel = sMinTaskLevel;
    bool overlaps = false;
    if (itRes.first != itRes.second) {
        overlaps = true;
        // it->first = Time Interval.
        // it->second = Number of overlaps in the interval.
        for (auto it = itRes.first; it != itRes.second; ++it) {
            if (it->second > mCurrentMaxTaskLevel) ++mCurrentMaxTaskLevel;
            if (it->second > thisTaskMinLevel) ++thisTaskMinLevel;
        }
    }
    const auto minTaskLevel = thisTaskMinLevel;
    //
    TaskWidget *taskWidget = new TaskWidget(info, minTaskLevel);
    mTaskWidgets << taskWidget;
    //
    if (!mColorPalette.empty()) {
        taskWidget->setFillColor(mColorPalette[info.funcID]);
    }
    //
    const qreal taskRight = stopTime;
    prepareGeometryChange();
    if (taskRight > mMaxX) mMaxX = taskRight;
    update();
    //
    if (oldMaxTaskLevel != mCurrentMaxTaskLevel) {
        mGraphWidget()->updateProcTimelineLayout();
    }
}

void
ProcTimeline::paint(
    QPainter *painter,
    const QStyleOptionGraphicsItem *option,
    QWidget *widget
) {
    const int widgetY = widget->pos().y();
    foreach (TaskWidget *tw, mTaskWidgets) {
        tw->setY(widgetY);
        tw->paint(painter, option, widget);
    }
    painter->setPen(Qt::black);
    const auto x1y1 = boundingRect().bottomLeft();
    const auto x2y2 = boundingRect().bottomRight();
    painter->drawLine(x1y1, x2y2);
}
