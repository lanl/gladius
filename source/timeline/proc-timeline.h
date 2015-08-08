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

#include <QList>
#include <QGraphicsItem>
#include <QPainter>
#include <QBrush>

#include <iostream>

#include <boost/icl/split_interval_map.hpp>

QT_BEGIN_NAMESPACE
class QRectF;
class QGraphicsView;
class QGraphicsLineItem;
QT_END_NAMESPACE

////////////////////////////////////////////////////////////////////////////////
class TaskWidget : public QGraphicsItem {
public:
    TaskWidget(
        const TaskInfo &info,
        uint32_t level
    ) : mInfo(info)
      , mLevel(level)
      , mWidth((mInfo.uStopTime - mInfo.uStartTime) / sMicroSecPerPixel)
      , mFillColor(Qt::gray /* Default Color */)
    {
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
        const QStyleOptionGraphicsItem * /* option */,
        QWidget * /* widget */) Q_DECL_OVERRIDE
    {
        painter->setPen(Qt::NoPen);
        painter->setBrush(mFillColor);
        painter->drawRect(boundingRect());
    }

    ustime_t
    getCreateTime(void) {
        return mInfo.uCreateTime;
    }

    ustime_t
    getReadyTime(void) {
        return mInfo.uReadyTime;
    }

    ustime_t
    getStartTime(void) {
        return mInfo.uStartTime;
    }

    ustime_t
    getStopTime(void) {
        return mInfo.uStopTime;
    }

    void
    setFillColor(const QColor &color) {
        mFillColor = color;
    }

    qreal
    getHeight(void) {
        return sHeight;
    }

private:
    //
    uint32_t mLevel = 0;
    //
    static constexpr qreal sHeight = 30;
    //
    TaskInfo mInfo;
    //
    qreal mWidth = 0.0;
    //
    QColor mFillColor;
};

////////////////////////////////////////////////////////////////////////////////
class ProcTimeline : public QGraphicsItem {
public:
    //
    ProcTimeline(
        ProcType procType,
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

    void
    debugDumpTimeIntervalData(void) {
        for (const auto &ti : mTimeIntervalMap) {
            std::cerr << "Time: " << ti.first
                      << " # Overlapping Times: "
                      << ti.second << std::endl;
        }
        std::cerr << std::endl;
    }

private:
    ProcType mProcType = ProcType::UNKNOWN;
    //
    QGraphicsView *mView = nullptr;
    //
    QRectF mBoundingRect;
    //
    QList<TaskWidget *> mTaskWidgets;
    //
    QList<QColor> mColorPalette;
    //
    QGraphicsLineItem *mTimeAxisLine = nullptr;
    //
    boost::icl::split_interval_map<ustime_t, uint32_t> mTimeIntervalMap;
};

#endif // TIMELINE_PROC_TIMELINE_H_INCLUDED
