/**
 * Copyright (c) 2015      Los Alamos National Security, LLC
 *                         All rights reserved.
 *
 * This file is part of the Gladius project. See the LICENSE.txt file at the
 * top-level directory of this distribution.
 */

#include "timeline-widget.h"
#include "proc-widget.h"

#include <QPainter>
#include <QDebug>

TimelineWidget::TimelineWidget(
    const LegionProfData &profData,
    QWidget *parent
) : QWidget(parent)
  , mProfData(profData)
{
    brush = QBrush(Qt::gray);
    pen = QPen(Qt::NoPen);
    //
    setBackgroundRole(QPalette::Base);
    setAutoFillBackground(true);

    mProcLayout = new QVBoxLayout();
    for (size_t p = 0; p < mProfData.nProcessors(); ++p) {
        ProcWidget *pw = new ProcWidget(p, this);
        mProcWidgets.push_back(pw);
        mProcLayout->addWidget(pw);
    }
    setLayout(mProcLayout);
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

/**
 * @brief TimelineWidget::paintEvent
 */
void
TimelineWidget::paintEvent(
    QPaintEvent * /* event */
) {
}
