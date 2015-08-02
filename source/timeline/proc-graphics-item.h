/**
 * Copyright (c) 2015      Los Alamos National Security, LLC
 *                         All rights reserved.
 *
 * This file is part of the Gladius project. See the LICENSE.txt file at the
 * top-level directory of this distribution.
 */

#ifndef PROC_GRAPHICS_ITEM_H_INCLUDED
#define PROC_GRAPHICS_ITEM_H_INCLUDED

#include <QGraphicsItem>
#include <QPainterPath>
#include <QRect>
#include <stdint.h>

class TimelineWidget;

class Proc : public QGraphicsItem {
public:
    //
    Proc(
        int64_t id,
        TimelineWidget *parent
    );
    //
    enum { Type = UserType + 1 };
    //
    int
    type(void) const Q_DECL_OVERRIDE {
        return Type;
    }
    //
    QRectF boundingRect(void) const Q_DECL_OVERRIDE;
    //
    QPainterPath shape(void) const Q_DECL_OVERRIDE;
    //
    void
    paint(
        QPainter *painter,
        const QStyleOptionGraphicsItem *option,
            QWidget *widget
    ) Q_DECL_OVERRIDE;

protected:
    QVariant
    itemChange(
        GraphicsItemChange change,
        const QVariant &value
    ) Q_DECL_OVERRIDE;
    //
    void
    mousePressEvent(
        QGraphicsSceneMouseEvent *event
    ) Q_DECL_OVERRIDE;
    //
    void
    mouseReleaseEvent(
        QGraphicsSceneMouseEvent *event
    ) Q_DECL_OVERRIDE;

private:
    //
    uint64_t mID = 0;
    //
    TimelineWidget *mTimeline = nullptr;
};

#endif // PROC_GRAPHICS_ITEM_H_INCLUDED
