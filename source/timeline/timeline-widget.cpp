/**
 * Copyright (c) 2015      Los Alamos National Security, LLC
 *                         All rights reserved.
 *
 * This file is part of the Gladius project. See the LICENSE.txt file at the
 * top-level directory of this distribution.
 */

#include "timeline-widget.h"

#include <QPainter>

TimelineWidget::TimelineWidget(
    QWidget *parent
) : QWidget(parent)
{
    brush = QBrush(Qt::gray);
    pen = QPen(Qt::NoPen);
    //
    setBackgroundRole(QPalette::Base);
    setAutoFillBackground(true);
}

QSize
TimelineWidget::minimumSizeHint() const
{
    return QSize(100, 100);
}

QSize
TimelineWidget::sizeHint() const
{
    return QSize(100, 100);
}

void
TimelineWidget::setPen(const QPen &pen)
{
    this->pen = pen;
    update();
}

void
TimelineWidget::setBrush(const QBrush &brush)
{
    this->brush = brush;
    update();
}

void
TimelineWidget::setAntialiased(bool antialiased)
{
    this->antialiased = antialiased;
    update();
}

void
TimelineWidget::setTransformed(bool transformed)
{
    this->transformed = transformed;
    update();
}

#include <QFile>
#include <QByteArray>
#include <QDebug>
#include <QRegExp>
#include <stdint.h>
#include <deque>
#include <QPainter>
#include <QGraphicsRectItem>

struct TaskInfo {
    uint32_t taskID = 0;
    //
    uint32_t funcID = 0;
    //
    uint64_t procID = 0;
    //
    uint64_t uCreateTime = 0;
    //
    uint64_t uReadyTime = 0;
    //
    uint64_t uStartTime = 0;
    //
    uint64_t uStopTime = 0;
    //
    TaskInfo(
        int32_t taskID,
        int32_t funcID,
        uint64_t procID,
        uint64_t uCreateTime,
        uint64_t uReadyTime,
        uint64_t uStartTime,
        uint64_t uStopTime
    ) : taskID(taskID)
      , funcID(funcID)
      , procID(procID)
      , uCreateTime(uCreateTime)
      , uReadyTime(uReadyTime)
      , uStartTime(uStartTime)
      , uStopTime(uStopTime) { }
};

struct ProcDesc {
    //
    uint64_t procID = 0;
    //
    uint32_t kind = 0;
    //
    ProcDesc(
        uint64_t procID,
        uint32_t kind
    ) : procID(procID)
      , kind(kind) { }
};


/**
 * @brief TimelineWidget::paintEvent
 */
void TimelineWidget::paintEvent(
    QPaintEvent * /* event */
) {
    QRegExp taskInfoRx(
        "Prof Task Info ([0-9]+) ([0-9]+) "
        "([0-9]+) ([0-9]+) ([0-9]+) ([0-9]+) ([0-9]+)"
    );
    std::deque<TaskInfo> taskInfos;
    //
    QRegExp procDescRx(
        "Prof Proc Desc ([0-9]+) ([0-9]+)"
    );
    std::deque<ProcDesc> procDescs;
    //
    QFile inputFile("/Users/samuel/OUT.prof");
    if (!inputFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "Cannot Open File!";
    }
    while (!inputFile.atEnd()) {
        QString line(inputFile.readLine());
        //
        if (taskInfoRx.indexIn(line) != -1) {
            taskInfos.push_back(
                TaskInfo(taskInfoRx.cap(1).toUInt(),
                         taskInfoRx.cap(2).toUInt(),
                         taskInfoRx.cap(3).toULongLong(),
                         taskInfoRx.cap(4).toULongLong(),
                         taskInfoRx.cap(5).toULongLong(),
                         taskInfoRx.cap(6).toULongLong(),
                         taskInfoRx.cap(7).toULongLong()
                )
            );
            continue;
        }
        if (procDescRx.indexIn(line) != -1) {
            procDescs.push_back(
                ProcDesc(procDescRx.cap(1).toULongLong(),
                         procDescRx.cap(2).toUInt()
                )
            );
            continue;
        }
    }
    inputFile.close();

    qDebug() << "# Procs" << procDescs.size();
    qDebug() << "Found " << taskInfos.size();

    QPainter painter(this);
    painter.setPen(pen);
    painter.setBrush(brush);
    if (antialiased) painter.setRenderHint(QPainter::Antialiasing, true);

    int x = 0;
    int y = height() / 2;
    qreal h = 32;
    static const uint32_t US_PER_PIXEL = 2048;
    for (const auto &taskInfo : taskInfos) {
        qreal w = (taskInfo.uStopTime - taskInfo.uStartTime) / US_PER_PIXEL;
        if (x >= width()) {
            resize(x + w , y);
        }
        painter.save();
        painter.translate(x, y);
        QRectF execRect(0, 0, w, h);
        painter.drawRect(execRect);
        x += w + 2;
        painter.restore();
    }
}
