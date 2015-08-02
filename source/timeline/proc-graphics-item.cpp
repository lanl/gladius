/**
 * Copyright (c) 2015      Los Alamos National Security, LLC
 *                         All rights reserved.
 *
 * This file is part of the Gladius project. See the LICENSE.txt file at the
 * top-level directory of this distribution.
 */

#include "proc-graphics-item.h"
#include "info-types.h"

#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>
#include <QPainter>
#include <QStyleOption>

#define PROC_H 32

Proc::Proc(
    int64_t id,
    TimelineWidget *parent
) : mID(id)
  , mTimeline(parent)
{
    setFlag(ItemSendsGeometryChanges);
    setCacheMode(DeviceCoordinateCache);
    setZValue(-1);
}

QRectF Proc::boundingRect() const
{
    qreal adjust = 2;
    return QRectF( -10 - adjust, -10 - adjust, 23 + adjust, 23 + adjust);
}

QPainterPath Proc::shape() const
{
    QPainterPath path;
    path.addRect(-10, -10, 20, 20);
    return path;
}

void
Proc::paint(
    QPainter *painter,
    const QStyleOptionGraphicsItem *option,
    QWidget *widget
) {
    painter->setBrush(Qt::red);
    painter->drawRect(-7, -7, 20, 20);
}

QVariant
Proc::itemChange(
    GraphicsItemChange change,
    const QVariant &value
) {

}

void
Proc::mousePressEvent(
    QGraphicsSceneMouseEvent *event
) {

}

void
Proc::mouseReleaseEvent(
QGraphicsSceneMouseEvent *event
) {

}

#if 0
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
#endif
