/**
 * Copyright (c) 2015      Los Alamos National Security, LLC
 *                         All rights reserved.
 *
 * This file is part of the Gladius project. See the LICENSE.txt file at the
 * top-level directory of this distribution.
 */

#include "main-frame.h"
#include "graph-widget.h"
#include "legion-prof-log-parser.h"

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

MainFrame::MainFrame(
    QWidget *parent
) : QFrame(parent)
{
    // No frame around us.
    setFrameShape(QFrame::NoFrame);
    //
    mGraphWidget = new GraphWidget(this);
    //
    mZoomSlider = new QSlider(this);
    mZoomSlider->setMinimum(sMinSliderValue);
    mZoomSlider->setMaximum(sMaxSliderValue);
    mZoomSlider->setValue(sInitSliderValue);
    mZoomSlider->setTickPosition(QSlider::TicksRight);
    // Zoom slider layout
    QVBoxLayout *zoomSliderLayout = new QVBoxLayout();
    zoomSliderLayout->addWidget(mZoomSlider);
    //
    mResetButton = new QToolButton(this);
    mResetButton->setText(tr("0"));
    mResetButton->setEnabled(false);
    //
    QGridLayout *topLayout = new QGridLayout(this);
    topLayout->addWidget(mGraphWidget, 1, 0);
    topLayout->addLayout(zoomSliderLayout, 1, 1);
    topLayout->addWidget(mResetButton, 2, 1);
    setLayout(topLayout);
    // TODO FIXME
    plotFromLogFile();
    //
    connect(
        mResetButton,
        SIGNAL(clicked()),
        this,
        SLOT(resetView())
    );
    //
    connect(
        mZoomSlider,
        SIGNAL(valueChanged(int)),
        this,
        SLOT(setupMatrix())
    );
    //
    connect(
        mGraphWidget->verticalScrollBar(),
        SIGNAL(valueChanged(int)),
        this,
        SLOT(setResetButtonEnabled())
    );
    //
    connect(
        mGraphWidget->horizontalScrollBar(),
        SIGNAL(valueChanged(int)),
        this,
        SLOT(setResetButtonEnabled())
    );
    //
    setupMatrix();
}

void
MainFrame::resetView(void)
{
    mZoomSlider->setValue(sInitSliderValue);
    setupMatrix();
    mGraphWidget->ensureVisible(QRectF(0, 0, 0, 0));
    mResetButton->setEnabled(false);
}

void
MainFrame::setResetButtonEnabled(void)
{
    mResetButton->setEnabled(true);
}

void
MainFrame::setupMatrix(void)
{
    qreal scale = qPow(
        qreal(2),
        (mZoomSlider->value() - sInitSliderValue) / qreal(50)
    );
    //
    QMatrix matrix;
    matrix.scale(scale, scale);
    //
    mGraphWidget->setMatrix(matrix);
    setResetButtonEnabled();
}

void
MainFrame::print(void)
{
#if !defined(QT_NO_PRINTER) && !defined(QT_NO_PRINTDIALOG)
    QPrinter printer;
    QPrintDialog dialog(&printer, this);
    if (dialog.exec() == QDialog::Accepted) {
        QPainter painter(&printer);
        mGraphWidget->render(&painter);
    }
#endif
}

void
MainFrame::plotFromLogFile(void)
{
    // TODO Add progress bar...
    qDebug() << "Loading Log File...";
    //
    LegionProfLogParser parser;
    parser.parse("/Users/samuel/OUT.prof");
    if (!parser.parseSuccessful()) {
        // TODO Display Bad Parse and Why
        qDebug() << "Bad Parse!";
    }
    // We have all the data we need, so just plot the thing.
    mGraphWidget->plot(parser.results());
}

void
MainFrame::zoomIn(int level)
{
    mZoomSlider->setValue(mZoomSlider->value() + level);
}

void
MainFrame::zoomOut(int level)
{
    mZoomSlider->setValue(mZoomSlider->value() - level);
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
