/**
 * Copyright (c) 2015      Los Alamos National Security, LLC
 *                         All rights reserved.
 *
 * This file is part of the Gladius project. See the LICENSE.txt file at the
 * top-level directory of this distribution.
 */

#ifndef TIMELINE_VIEW_H_INCLUDED
#define TIMELINE_VIEW_H_INCLUDED

#include "scene.h"

#include <QFrame>
#include <QGraphicsView>

QT_BEGIN_NAMESPACE
class QLabel;
class QSlider;
class QToolButton;
QT_END_NAMESPACE

/**
 * @brief The View class
 */
class MainFrame : public QFrame {
    Q_OBJECT
public:
    explicit MainFrame(QWidget *parent = nullptr);
    //
    QGraphicsView *view(void) const;

public slots:
    void zoomIn(int level = 1);
    //
    void zoomOut(int level = 1);

protected:
    void wheelEvent(QWheelEvent *e) Q_DECL_OVERRIDE;

private slots:
    void resetView(void);
    //
    void setResetButtonEnabled(void);
    //
    void setupMatrix(void);
    //
    void togglePointerMode(void);
    //
    void toggleAntialiasing(void);
    //
    void print(void);

private:
    QGraphicsView *mGraphicsView = nullptr;
    //
    QToolButton *resetButton = nullptr;
    //
    QSlider *zoomSlider = nullptr;
    //
    Scene *mScene = nullptr;

    static constexpr int sMinSliderValue  = 0;
    static constexpr int sMaxSliderValue  = 512;
    static constexpr int sInitSliderValue = sMaxSliderValue / 2;
};

#endif // TIMELINE_VIEW_H_INCLUDED