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
    // SKG Debug Views
    setBackgroundRole(QPalette::Foreground);
    setAutoFillBackground(true);

    mProcLayout = new QVBoxLayout();
    for (size_t p = 0; p < mProfData.nProcessors(); ++p) {
        ProcWidget *pw = new ProcWidget(p, this);
        mProcWidgets.push_back(pw);
        mProcLayout->addWidget(pw);
    }
    setLayout(mProcLayout);
}

/**
 * @brief TimelineWidget::paintEvent
 */
void
TimelineWidget::paintEvent(
    QPaintEvent * /* event */
) {
}
