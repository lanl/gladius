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
    mZoomValue = sInitZoomValue;
    // No frame around us.
    setFrameShape(QFrame::NoFrame);
    //
    mGraphWidget = new GraphWidget(this);
    //
    QGridLayout *layout = new QGridLayout(this);
    layout ->addWidget(mGraphWidget, 1, 0);
    setLayout(layout);
    // TODO FIXME
    plotFromLogFile();
    //
    setupMatrix();
}

void
MainFrame::resetView(void)
{
    mZoomValue = sInitZoomValue;
    setupMatrix();
    //mGraphWidget->ensureVisible(QRectF(0, 0, 0, 0));
}

void
MainFrame::setupMatrix(void)
{
    qreal scale = qPow(
        qreal(2),
        (mZoomValue - sInitZoomValue) / qreal(50)
    );
    //
    QMatrix matrix;
    matrix.scale(scale, scale);
    //
    mGraphWidget->setMatrix(matrix);
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
MainFrame::keyPressEvent(
    QKeyEvent *keyEvent
) {
    switch (keyEvent->key()) {
        // Reset Zoom
        case '0': {
            resetView();
            break;
        }
        // Zoom In
        case '+': {
            if (mZoomValue < sMaxZoomValue) {
                mZoomValue += sZoomKeyIncrement;
                setupMatrix();
            }
            break;
        }
        // Zoom Out
        case '-': {
            if (mZoomValue > sMinZoomValue) {
                mZoomValue -= sZoomKeyIncrement;
                setupMatrix();
            }
            break;
        }
        default: QFrame::keyPressEvent(keyEvent);
    }
}
