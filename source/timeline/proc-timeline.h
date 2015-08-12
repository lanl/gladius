/**
 * Copyright (c) 2015      Los Alamos National Security, LLC
 *                         All rights reserved.
 *
 * This file is part of the Gladius project. See the LICENSE.txt file at the
 * top-level directory of this distribution.
 */

#ifndef TIMELINE_PROC_TIMELINE_H_INCLUDED
#define TIMELINE_PROC_TIMELINE_H_INCLUDED

#include "common.h"
#include "info-types.h"
#include "graph-widget.h"

#include <QList>
#include <QGraphicsItem>
#include <QPainter>
#include <QBrush>
#include <QStyleOptionGraphicsItem>

#include <iostream>

#include <boost/icl/split_interval_map.hpp>

QT_BEGIN_NAMESPACE
class QRectF;
class QGraphicsView;
class QGraphicsLineItem;
QT_END_NAMESPACE

class TaskWidget;

////////////////////////////////////////////////////////////////////////////////
class ProcTimeline : public QGraphicsItem {
public:
    //
    ProcTimeline(
        const ProcDesc &procDesc,
        QGraphicsView *parent
    );
    //
    QRectF boundingRect(void) const Q_DECL_OVERRIDE;
    //
    void
    paint(
        QPainter *painter,
        const QStyleOptionGraphicsItem *option,
        QWidget *widget
    ) Q_DECL_OVERRIDE;
    //
    void
    addTask(const TaskInfo &info);
    //
    void
    setTaskColorPalette(const QList<QColor> &colorPalette) {
        mColorPalette = colorPalette;
    }
    //
    void
    debugDumpTimeIntervalData(void) {
        for (const auto &ti : mTimeIntervalMap) {
            std::cerr << "Time: " << ti.first
                      << " # Overlapping Times: "
                      << ti.second << std::endl;
        }
        std::cerr << std::endl;
    }
    //
    void
    propagatePositionUpdate(void);


private:
    //
    ProcDesc mProcDesc;
    //
    qreal mMaxX = 0.0;
    //
    QGraphicsView *mView = nullptr;
    //
    QList<TaskWidget *> mTaskWidgets;
    //
    QList<QColor> mColorPalette;
    // A map of time intervals (in ustime_t) and number of overlaps at
    // a given interval.
    boost::icl::split_interval_map<ustime_t, uint32_t> mTimeIntervalMap;
    //
    static constexpr uint16_t sMinTaskLevel = 1;
    // If the number of concurrent threads exceeds 2^16, then wow...
    uint16_t mCurrentMaxTaskLevel = 0;
    //
    GraphWidget *
    mGraphWidget(void) const {
        return static_cast<GraphWidget *>(mView);
    }

};

////////////////////////////////////////////////////////////////////////////////
class TaskWidget : public QGraphicsItem {
public:
    TaskWidget(
        const TaskInfo &info,
        uint16_t level,
        ProcTimeline *timeline
    ) : mInfo(info)
      , mLevel(level - 1 /* Comes in at base 1 and we want base 0 */)
      , mWidth((mInfo.uStopTime - mInfo.uStartTime) / sMicroSecPerPixel)
      , mFillColor(Qt::gray /* Default Color */)
    {
        setPos(mInfo.uStartTime / sMicroSecPerPixel, timeline->pos().y());
        //
        setFlags(ItemIsSelectable);
        //
        setAcceptHoverEvents(true);
        // TODO Add Cache
        QString toolTip = "Start:" + QString::number(mInfo.uStartTime)
                        + " End: " + QString::number(mInfo.uStopTime);
        setToolTip(toolTip);
    }
    //
    QRectF boundingRect(void) const Q_DECL_OVERRIDE {
        return QRectF(0, 0, mWidth, sHeight);
    }
    //
    void
    paint(
        QPainter *painter,
        const QStyleOptionGraphicsItem *option,
        QWidget * /* widget */) Q_DECL_OVERRIDE
    {
        painter->setPen(Qt::NoPen);
        const QColor fillColor = (option->state & QStyle::State_Selected)
                                 ? mFillColor.light(128) : mFillColor;
        painter->setBrush(fillColor);
        painter->drawRect(boundingRect());
    }
    //
    ustime_t
    getCreateTime(void) const {
        return mInfo.uCreateTime;
    }
    //
    ustime_t
    getReadyTime(void) const {
        return mInfo.uReadyTime;
    }
    //
    ustime_t
    getStartTime(void) const {
        return mInfo.uStartTime;
    }
    //
    ustime_t
    getStopTime(void) const {
        return mInfo.uStopTime;
    }
    //
    void
    setFillColor(const QColor &color) {
        mFillColor = color;
    }
    //
    static qreal
    getHeight(void) {
        return sHeight;
    }
    //
    uint16_t getLevel(void) const {
        return mLevel;
    }

private:
    //
    TaskInfo mInfo;
    //
    uint16_t mLevel = 0;
    //
    qreal mWidth = 0.0;
    //
    static constexpr qreal sHeight = 30;
    //
    QColor mFillColor;
};


#endif // TIMELINE_PROC_TIMELINE_H_INCLUDED
