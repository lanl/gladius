/**
 * Copyright (c) 2015      Los Alamos National Security, LLC
 *                         All rights reserved.
 *
 * This file is part of the Gladius project. See the LICENSE.txt file at the
 * top-level directory of this distribution.
 */

#ifndef PROC_WIDGET_H_INCLUDED
#define PROC_WIDGET_H_INCLUDED

#include <QBrush>
#include <QPen>
#include <QWidget>

#include <stdint.h>

class ProcWidget : public QWidget {
    Q_OBJECT

public:
    ProcWidget(uint64_t id, QWidget *parent = nullptr);
    //
    QSize minimumSizeHint() const Q_DECL_OVERRIDE;
    //
    QSize sizeHint() const Q_DECL_OVERRIDE;

public slots:
    void setPen(const QPen &pen);
    //
    void setBrush(const QBrush &brush);

protected:
    void paintEvent(QPaintEvent *event) Q_DECL_OVERRIDE;

private:
    uint64_t mID = 0;
    //
    QPen pen;
    //
    QBrush brush;
    //
    bool transformed;
    //
    QSize mSize;
};

#endif // PROC_WIDGET_H_INCLUDED
