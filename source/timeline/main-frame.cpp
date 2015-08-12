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

#include <QFileDialog>
#include <QString>
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
    //
    setupMatrix();
}

void
MainFrame::resetView(void)
{
    mZoomValue = sInitZoomValue;
    setupMatrix();
    // TODO For streaming data, make sure that the right side is visible.
    //mGraphWidget->ensureVisible(QRectF(0, 0, 0, 0));
}

void
MainFrame::setupMatrix(void)
{
    qreal scale = qPow(
        2.0,
        (mZoomValue - sInitZoomValue) / 50.0
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
MainFrame::plotFromLogFile(
    const QString &fileName
) {
    // TODO Add progress bar...
    qDebug() << "Loading Log File...";
    //
    LegionProfLogParser parser;
    parser.parse(fileName);
    if (!parser.parseSuccessful()) {
        // TODO Display Bad Parse and Why
        qDebug() << "Bad Parse!";
    }
    // We have all the data we need, so just plot the thing.
    mGraphWidget->plot(parser.results());
}

QString
MainFrame::openLogFile(void)
{
    QString fileName = QFileDialog::getOpenFileName(
        this,
        tr("Open Log File"),
        QDir::homePath(),
        tr("Log Files (*.*)")
    );
    //
    return fileName;
}

void
MainFrame::keyPressEvent(
    QKeyEvent *keyEvent
) {
    const bool commandPressed = (keyEvent->modifiers() & Qt::ControlModifier);
    // Open Log File
    if (keyEvent->matches(QKeySequence::Open)) {
        QString fileName = openLogFile();
        // TODO also check if we need to cleanup old plot.
        if (!fileName.isEmpty()) {
            plotFromLogFile(fileName);
        }
        // Done in either case.
        return;
    }
    // Zoom In
    else if (keyEvent->matches(QKeySequence::ZoomIn) ||
             (commandPressed && keyEvent->key() == Qt::Key_Equal)) {
        if (mZoomValue < sMaxZoomValue) {
            mZoomValue += sZoomKeyIncrement;
            setupMatrix();
        }
        return;
    }
    // Zoom Out
    else if (keyEvent->matches(QKeySequence::ZoomOut)) {
        if (mZoomValue > sMinZoomValue) {
            mZoomValue -= sZoomKeyIncrement;
            setupMatrix();
        }
        return;
    }
    // Print
    else if (keyEvent->matches(QKeySequence::Print)) {
        print();
        return;
    }
    switch (keyEvent->key()) {
        // Reset Zoom
        case Qt::Key_Equal: {
            resetView();
            break;
        }
        default: QFrame::keyPressEvent(keyEvent);
    }
}
