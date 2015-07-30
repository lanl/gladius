/**
 * Copyright (c) 2015      Los Alamos National Security, LLC
 *                         All rights reserved.
 *
 * This file is part of the Gladius project. See the LICENSE.txt file at the
 * top-level directory of this distribution.
 */

#include "view.h"

#ifndef QT_NO_PRINTER
#include <QPrinter>
#include <QPrintDialog>
#endif
#ifndef QT_NO_OPENGL
#include <QtOpenGL>
#else
#include <QtWidgets>
#endif
#include <qmath.h>

#ifndef QT_NO_WHEELEVENT
void GraphicsView::wheelEvent(QWheelEvent *e)
{
    if (e->modifiers() & Qt::ControlModifier) {
        if (e->delta() > 0) view->zoomIn(6);
        else view->zoomOut(6);
        e->accept();
    }
    else {
        QGraphicsView::wheelEvent(e);
    }
}
#endif

// TODO: See setStyleSheet

/**
 * @brief View::View
 * @param name
 * @param parent
 */
View::View(
    QWidget *parent
) : QFrame(parent)
{
    // No frame around us.
    setFrameShape(QFrame::NoFrame);
    //
    mGraphicsView = new GraphicsView(this);
    mGraphicsView->setRenderHint(QPainter::Antialiasing, false);
    mGraphicsView->setDragMode(QGraphicsView::RubberBandDrag);
    mGraphicsView->setOptimizationFlags(QGraphicsView::DontSavePainterState);
    mGraphicsView->setViewportUpdateMode(QGraphicsView::SmartViewportUpdate);
    mGraphicsView->setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
    mGraphicsView->setFrameShape(QFrame::NoFrame);
    //graphicsView->setBackgroundBrush(QBrush(Qt::black));

    zoomSlider = new QSlider();
    zoomSlider->setMinimum(0);
    zoomSlider->setMaximum(500);
    zoomSlider->setValue(250);
    zoomSlider->setTickPosition(QSlider::TicksRight);

    // Zoom slider layout
    QVBoxLayout *zoomSliderLayout = new QVBoxLayout();
    zoomSliderLayout->addWidget(zoomSlider);

    resetButton = new QToolButton;
    resetButton->setText(tr("0"));
    resetButton->setEnabled(false);

    QGridLayout *topLayout = new QGridLayout();
    topLayout->addWidget(mGraphicsView, 1, 0);
    topLayout->addLayout(zoomSliderLayout, 1, 1);
    topLayout->addWidget(resetButton, 2, 1);
    setLayout(topLayout);

    connect(resetButton, SIGNAL(clicked()), this, SLOT(resetView()));
    connect(zoomSlider, SIGNAL(valueChanged(int)), this, SLOT(setupMatrix()));
    connect(mGraphicsView->verticalScrollBar(), SIGNAL(valueChanged(int)),
            this, SLOT(setResetButtonEnabled()));
    connect(mGraphicsView->horizontalScrollBar(), SIGNAL(valueChanged(int)),
            this, SLOT(setResetButtonEnabled()));
    setupMatrix();
}

/**
 * @brief View::view
 * @return
 */
QGraphicsView *
View::view() const
{
    return static_cast<QGraphicsView *>(mGraphicsView);
}

/**
 * @brief View::resetView
 */
void
View::resetView()
{
    zoomSlider->setValue(250);
    setupMatrix();
    mGraphicsView->ensureVisible(QRectF(0, 0, 0, 0));

    resetButton->setEnabled(false);
}

/**
 * @brief View::setResetButtonEnabled
 */
void
View::setResetButtonEnabled()
{
    resetButton->setEnabled(true);
}

/**
 * @brief View::setupMatrix
 */
void
View::setupMatrix()
{
    qreal scale = qPow(qreal(2), (zoomSlider->value() - 250) / qreal(50));

    QMatrix matrix;
    matrix.scale(scale, scale);

    mGraphicsView->setMatrix(matrix);
    setResetButtonEnabled();
}

/**
 * @brief View::togglePointerMode
 */
void
View::togglePointerMode()
{
}


/**
 * @brief View::toggleAntialiasing
 */
void
View::toggleAntialiasing()
{
}

/**
 * @brief View::print
 */
void
View::print()
{
#if !defined(QT_NO_PRINTER) && !defined(QT_NO_PRINTDIALOG)
    QPrinter printer;
    QPrintDialog dialog(&printer, this);
    if (dialog.exec() == QDialog::Accepted) {
        QPainter painter(&printer);
        mGraphicsView->render(&painter);
    }
#endif
}

/**
 * @brief View::zoomIn
 * @param level
 */
void
View::zoomIn(int level)
{
    zoomSlider->setValue(zoomSlider->value() + level);
}

/**
 * @brief View::zoomOut
 * @param level
 */
void
View::zoomOut(int level)
{
    zoomSlider->setValue(zoomSlider->value() - level);
}
