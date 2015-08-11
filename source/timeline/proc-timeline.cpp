/**
 * Copyright (c) 2015      Los Alamos National Security, LLC
 *                         All rights reserved.
 *
 * This file is part of the Gladius project. See the LICENSE.txt file at the
 * top-level directory of this distribution.
 */

#include "common.h"
#include "proc-timeline.h"

#include <QString>
#include <QBrush>
#include <QDebug>
#include <QPainter>
#include <QGraphicsView>

namespace {
//
QString
procType2QString(ProcType pType) {
    switch (pType) {
        case TOC_PROC  : return "GPU";
        case LOC_PROC  : return "CPU";
        case UTIL_PROC : return "Utility";
        case IO_PROC   : return "IO";
        case PROC_GROUP: return "Proc Group";
        case UNKNOWN   : return "Unknown";
        default        : Q_ASSERT(false);
    }
}

} // end namespace

ProcTimeline::ProcTimeline(
    const ProcDesc &procDesc,
    QGraphicsView *parent
) : mProcDesc(procDesc)
  , mView(parent)
  , mCurrentMaxTaskLevel(1) { }

QRectF
ProcTimeline::boundingRect(void) const
{
    if (mTaskWidgets.empty()) return QRectF();
    //
    static const qreal spaceForXTimeline = 5;
    return QRectF(
        0,
        0,
        mMaxX,
        (mCurrentMaxTaskLevel * TaskWidget::getHeight()) + spaceForXTimeline
    );
}

void
ProcTimeline::addTask(
    const TaskInfo &info
) {
    using namespace boost::icl;

    const ustime_t startTime = info.uStartTime;
    const ustime_t stopTime  = info.uStopTime;
    //
    boost::icl::interval<ustime_t>::type window;
    window = interval<ustime_t>::closed(startTime, stopTime);
    // The 1 here is the increment when there is an overlap.
    mTimeIntervalMap.add(std::make_pair(window, 1));
    // Now figure out at what level the task will be drawn. We do this be first
    // determining whether or not there exists overlapping ranges. If so,
    // now determine the max number of overlaps.
    auto itRes = mTimeIntervalMap.equal_range(window);
    // Stash this so we know when to call updateProcTimelineLayout.
    const auto oldMaxTaskLevel = mCurrentMaxTaskLevel;
    auto curTaskMinLevel = sMinTaskLevel;
    // it->first = Time Interval.
    // it->second = Number of overlaps in the interval.
    for (auto it = itRes.first; it != itRes.second; ++it) {
        if (it->second > mCurrentMaxTaskLevel) {
            mCurrentMaxTaskLevel = it->second;
        }
        if (it->second > curTaskMinLevel) {
            curTaskMinLevel = it->second;
        }
    }
    //
    TaskWidget *taskWidget = new TaskWidget(info, curTaskMinLevel, this);
    mTaskWidgets << taskWidget;
    mView->scene()->addItem(taskWidget);
    //
    if (!mColorPalette.empty()) {
        taskWidget->setFillColor(mColorPalette[info.funcID]);
    }
    //
    const qreal taskRight = stopTime / sMicroSecPerPixel;
    prepareGeometryChange();
    if (taskRight > mMaxX) mMaxX = taskRight;
    update();
    //
    if (oldMaxTaskLevel != mCurrentMaxTaskLevel) {
        mGraphWidget()->updateProcTimelineLayout();
    }
}

void
ProcTimeline::propagatePositionUpdate(void)
{
    const int taskY = pos().y();
    foreach (TaskWidget *tw, mTaskWidgets) {
        tw->setY(taskY + (tw->getLevel() * TaskWidget::getHeight()));
    }
}

void
ProcTimeline::paint(
    QPainter *painter,
    const QStyleOptionGraphicsItem *option,
    QWidget *widget
) {
    Q_UNUSED(painter);
    Q_UNUSED(option);
    Q_UNUSED(widget);
    //
    painter->save();
    //
    const auto x1y1 = boundingRect().bottomLeft();
    const auto x2y2 = boundingRect().bottomRight();
    // Draw Timeline
    painter->setPen(Qt::black);
    painter->drawLine(x1y1, x2y2);
    // Draw Timeline Legend
    static const int legendFixup = -1;
    const auto procIDStr = QString("%1").arg(
        mProcDesc.procID, 6, 10, QChar('0')
    );
    const auto timelineLegend = procType2QString(mProcDesc.kind)
                              + " " + procIDStr;
    painter->drawText(x1y1.x(), x1y1.y() + legendFixup, timelineLegend);
    //
    painter->restore();
}
