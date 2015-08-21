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
    static const qreal spaceForXTimeline = 5.0;
    static const qreal minWidth = 1e2;
    //
    if (mTaskWidgets.empty()) {
        return QRectF(
            0,
            0,
            minWidth,
            TaskWidget::getHeight() + spaceForXTimeline
        );
    }
    //
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
    const ustime_t duration = stopTime - startTime;
    // No point in drawing something this small, so skip all the work...
    if (duration < sMicroSecPerPixel) return;
    //
    boost::icl::interval<ustime_t>::type window;
    window = interval<ustime_t>::open(startTime, stopTime);
    // The 1 here is the increment when there is an overlap.
    mTimeIntervalMap.add(std::make_pair(window, 1));
    // Now figure out at what level the task will be drawn. We do this be first
    // determining whether or not there exists overlapping ranges. If so,
    // now determine the max number of overlaps.
    const auto itRes = mTimeIntervalMap.equal_range(window);
    // Stash this so we know when to call updateProcTimelineLayout.
    const auto oldMaxTaskLevel = mCurrentMaxTaskLevel;
    auto curTaskMinLevel = sMinTaskLevel;
    qreal zVal = 0.0;
    // it->first = Time Interval.
    // it->second = Number of overlaps in the interval.
    for (auto it = itRes.first; it != itRes.second; ++it) {
        if (boost::icl::within(window, it->first)) {
            if (++curTaskMinLevel > mCurrentMaxTaskLevel) {
                mCurrentMaxTaskLevel = curTaskMinLevel;
            }
        }
        // Send it to the back a bit. This is the case where what we are
        // plotting completely covers what's already placed. Instead of
        // reordering things, we just send the larger item to the back a bit.
        else if (boost::icl::contains(window, it->first)) {
            zVal -= 0.1;
        }
    }
    // Update bounding rectangle width if need be.
    const qreal taskRight = stopTime / sMicroSecPerPixel;
    const bool widthUpdated = taskRight > mMaxX;
    //
    if (widthUpdated) {
        mMaxX = taskRight;
    }
    // Did our bounding rectangle change?
    if (oldMaxTaskLevel != mCurrentMaxTaskLevel || widthUpdated) {
        prepareGeometryChange();
        update();
    }
    //
    TaskWidget *taskWidget = new TaskWidget(
        info,
        curTaskMinLevel - 1,
        zVal,
        this
    );
    if (!mColorPalette.empty()) {
        taskWidget->setFillColor(mColorPalette[info.funcID]);
    }
    mTaskWidgets << taskWidget;
    mView->scene()->addItem(taskWidget);
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
    painter->setPen(Qt::gray);
    painter->drawLine(x1y1, x2y2);
    // Draw Timeline Legend
    static const int legendFixup = -1;
    const auto procIDStr = QString("%1").arg(
        mProcDesc.procID, 6, 10, QChar('0')
    );
    // Draw legend.
    const auto timelineLegend = procType2QString(mProcDesc.kind)
                              + " " + procIDStr;
    painter->drawText(x1y1.x(), x1y1.y() + legendFixup, timelineLegend);
    //
    // Draw Time Tick Marks. One every 20 milliseconds.
    painter->setPen(Qt::black);
    static const uint32_t tickIncrement = 2 * 1e4 / sMicroSecPerPixel;
    static const uint8_t majorTickLen = 4;
    for (uint64_t t = 0; t < boundingRect().width(); t += tickIncrement) {
        painter->drawLine(
            t,
            x1y1.y() - majorTickLen,
            t,
            x1y1.y() + majorTickLen
        );
    }
    painter->restore();
}
