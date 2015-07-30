/**
 * Copyright (c) 2015      Los Alamos National Security, LLC
 *                         All rights reserved.
 *
 * This file is part of the Gladius project. See the LICENSE.txt file at the
 * top-level directory of this distribution.
 */

#include "proc-widget.h"
#include "info-types.h"

#include <QPainter>
#include <QDebug>

#define PROC_H 32

ProcWidget::ProcWidget(
    uint64_t id,
    QWidget *parent
) : QWidget(parent)
  , mID(id)
{
    brush = QBrush(Qt::gray);
    pen = QPen(Qt::NoPen);
    //
    setBackgroundRole(QPalette::Base);
    setAutoFillBackground(true);
}

QSize
ProcWidget::minimumSizeHint() const
{
    return QSize(128, PROC_H);
}

QSize
ProcWidget::sizeHint() const
{
    return minimumSizeHint();
}

void
ProcWidget::setPen(const QPen &pen)
{
    this->pen = pen;
    update();
}

void
ProcWidget::setBrush(const QBrush &brush)
{
    this->brush = brush;
    update();
}

/**
 * @brief TimelineWidget::paintEvent
 */
void
ProcWidget::paintEvent(
    QPaintEvent *event
) {

    QPainter painter(this);
    painter.setPen(pen);
    painter.setBrush(brush);

    int x = 0;
    int y = height() / 2;
    qreal h = 32;
    static const uint32_t US_PER_PIXEL = 2048;
    painter.drawRect(QRectF(0, 0, 20, 20));
#if 0
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
#endif
}
