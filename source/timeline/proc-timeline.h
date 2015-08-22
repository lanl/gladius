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
    doneAddingTasks(void);
    //
    void
    setTaskColorPalette(const QList<QColor> &colorPalette) {
        mColorPalette = colorPalette;
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
    // a given interval. Inclusive, so no overlap is not 0, it's 1.
    boost::icl::split_interval_map<ustime_t, uint32_t> *mTimeIntervals;
    // Since mTimeIntervalMap is inclusive, 1 is the minimum level.
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
        qreal zVal,
        ProcTimeline *timeline
    ) : mInfo(info)
      , mLevel(level)
      , mZValueStash(zVal)
      , mWidth((mInfo.uStopTime - mInfo.uStartTime) / sMicroSecPerPixel)
      , mColor(Qt::gray /* Default Color */)
      , mLightColor(mColor.light(sLightness))
    {
        setPos(mInfo.uStartTime / sMicroSecPerPixel, timeline->pos().y());
        //
        setFlags(ItemIsSelectable);
        //
        setAcceptHoverEvents(true);
        // TODO Add Cache
        QString toolTip = "Start:" + QString::number(mInfo.uStartTime)
                        + " End: " + QString::number(mInfo.uStopTime)
                        + " Level: " + QString::number(mLevel);
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
        const bool selected = option->state & QStyle::State_Selected;
        //
        const QColor penColor = selected ? mColor : mLightColor;
        const QColor fillColor = selected ? mLightColor : mColor;
        // Bring foward if selected.
        if (selected)  setZValue(sMaxZVal);
        else  setZValue(mZValueStash);
        //
        painter->setPen(penColor);
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
        mColor = color;
        mLightColor = mColor.light(sLightness);
    }
    //
    static qreal
    getHeight(void) {
        return sHeight;
    }
    //
    uint16_t
    getLevel(void) const {
        return mLevel;
    }
    //
    void
    updateZValue(qreal newZVal) {
        mZValueStash = newZVal;
        setZValue(mZValueStash);
    }

private:
    TaskInfo mInfo;
    //
    uint16_t mLevel = 0;
    //
    qreal mZValueStash = 0.0;
    //
    qreal mWidth = 0.0;
    //
    static constexpr qreal sHeight = 30;
    //
    static constexpr int sLightness = 128;
    //
    static constexpr qreal sMaxZVal = 10.0;
    //
    QColor mColor;
    //
    QColor mLightColor;
};

#endif // TIMELINE_PROC_TIMELINE_H_INCLUDED
