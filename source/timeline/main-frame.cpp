/**
 * Copyright (c) 2015      Los Alamos National Security, LLC
 *                         All rights reserved.
 *
 * This file is part of the Gladius project. See the LICENSE.txt file at the
 * top-level directory of this distribution.
 */

#include "main-frame.h"

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

// TODO: See setStyleSheet

/**
 * @brief View::View
 * @param name
 * @param parent
 */
MainFrame::MainFrame(
    QWidget *parent
) : QFrame(parent)
{
    // No frame around us.
    setFrameShape(QFrame::NoFrame);
    //
    mGraphicsView = new QGraphicsView(this);
    mGraphicsView->setRenderHint(QPainter::Antialiasing, false);
    mGraphicsView->setDragMode(QGraphicsView::RubberBandDrag);
    mGraphicsView->setOptimizationFlags(QGraphicsView::DontSavePainterState);
    mGraphicsView->setViewportUpdateMode(QGraphicsView::SmartViewportUpdate);
    mGraphicsView->setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
    mGraphicsView->setFrameShape(QFrame::NoFrame);
    mGraphicsView->setBackgroundBrush(QBrush(Qt::gray));
    //
    zoomSlider = new QSlider(this);
    zoomSlider->setMinimum(sMinSliderValue);
    zoomSlider->setMaximum(sMaxSliderValue);
    zoomSlider->setValue(sInitSliderValue);
    zoomSlider->setTickPosition(QSlider::TicksRight);
    // Zoom slider layout
    QVBoxLayout *zoomSliderLayout = new QVBoxLayout();
    zoomSliderLayout->addWidget(zoomSlider);
    //
    resetButton = new QToolButton(this);
    resetButton->setText(tr("0"));
    resetButton->setEnabled(false);
    //
    QGridLayout *topLayout = new QGridLayout(this);
    topLayout->addWidget(mGraphicsView, 1, 0);
    topLayout->addLayout(zoomSliderLayout, 1, 1);
    topLayout->addWidget(resetButton, 2, 1);
    setLayout(topLayout);
    //
    mScene = new Scene(this);
    view()->setScene(mScene);
    //
    connect(
        resetButton,
        SIGNAL(clicked()),
        this,
        SLOT(resetView())
    );
    //
    connect(
        zoomSlider,
        SIGNAL(valueChanged(int)),
        this,
        SLOT(setupMatrix())
    );
    //
    connect(
        mGraphicsView->verticalScrollBar(),
        SIGNAL(valueChanged(int)),
        this,
        SLOT(setResetButtonEnabled())
    );
    //
    connect(
        mGraphicsView->horizontalScrollBar(),
        SIGNAL(valueChanged(int)),
        this,
        SLOT(setResetButtonEnabled())
    );
    //
    setupMatrix();
}

/**
 * @brief View::view
 * @return
 */
QGraphicsView *
MainFrame::view(void) const
{
    return static_cast<QGraphicsView *>(mGraphicsView);
}

/**
 * @brief View::resetView
 */
void
MainFrame::resetView(void)
{
    zoomSlider->setValue(sInitSliderValue);
    setupMatrix();
    mGraphicsView->ensureVisible(QRectF(0, 0, 0, 0));
    resetButton->setEnabled(false);
}

/**
 * @brief View::setResetButtonEnabled
 */
void
MainFrame::setResetButtonEnabled(void)
{
    resetButton->setEnabled(true);
}

/**
 * @brief View::setupMatrix
 */
void
MainFrame::setupMatrix(void)
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
MainFrame::togglePointerMode(void)
{
}


/**
 * @brief View::toggleAntialiasing
 */
void
MainFrame::toggleAntialiasing(void)
{
}

/**
 * @brief View::print
 */
void
MainFrame::print(void)
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
MainFrame::zoomIn(int level)
{
    zoomSlider->setValue(zoomSlider->value() + level);
}

/**
 * @brief View::zoomOut
 * @param level
 */
void
MainFrame::zoomOut(int level)
{
    zoomSlider->setValue(zoomSlider->value() - level);
}

void
MainFrame::wheelEvent(QWheelEvent *e)
{
    if (e->modifiers() & Qt::ControlModifier) {
        if (e->delta() > 0) zoomIn(6);
        else zoomOut(6);
        e->accept();
    }
    else {
        QFrame::wheelEvent(e);
    }
}
