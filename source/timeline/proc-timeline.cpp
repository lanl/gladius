/**
 * Copyright (c) 2015      Los Alamos National Security, LLC
 *                         All rights reserved.
 *
 * This file is part of the Gladius project. See the LICENSE.txt file at the
 * top-level directory of this distribution.
 */

#include "proc-timeline.h"

#include <QBrush>
#include <QDebug>
#include <QPainter>
#include <QGraphicsView>

ProcTimeline::ProcTimeline(
    ProcType procType,
    QGraphicsView *parent
) : mProcType(procType)
  , mView(parent)
{
    setFlag(ItemSendsGeometryChanges);
}

QRectF
ProcTimeline::boundingRect(void) const
{
    if (mTaskWidgets.empty()) return QRectF();
}

void
ProcTimeline::addTask(
    const TaskInfo &info
) {
    TaskWidget *taskWidget = new TaskWidget(info);
    qreal x = 0.0, y = pos().y();
    x = qreal(info.uStartTime / 100);
    taskWidget->setPos(x, y);
    mTaskWidgets << taskWidget;
    mView->scene()->addItem(taskWidget);
}

void
ProcTimeline::paint(
    QPainter *painter,
    const QStyleOptionGraphicsItem *option,
    QWidget *widget
) {
}
