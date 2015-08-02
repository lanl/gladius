/**
 * Copyright (c) 2015      Los Alamos National Security, LLC
 *                         All rights reserved.
 *
 * This file is part of the Gladius project. See the LICENSE.txt file at the
 * top-level directory of this distribution.
 */

#ifndef TIMELINE_WIDGET_H_INCLUDED
#define TIMELINE_WIDGET_H_INCLUDED

#include "proc-graphics-item.h"
#include "info-types.h"

#include <QBrush>
#include <QPen>
#include <QWidget>
#include <QVBoxLayout>

#include <deque>

class TimelineWidget : public QWidget {
    Q_OBJECT

public:
    TimelineWidget(
        const LegionProfData &profData,
        QWidget *parent = nullptr
    );

protected:
    void paintEvent(QPaintEvent *event) Q_DECL_OVERRIDE;

private:
    //
    LegionProfData mProfData;
    //
    std::deque<Proc *> mProcWidgets;
    //
    QVBoxLayout *mProcLayout = nullptr;
};

#endif // TIMELINE_WIDGET_H_INCLUDED
