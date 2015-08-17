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

#include <QtCore>
#include <QtConcurrent>
#include <QThread>
#include <QFileDialog>
#include <QString>
#include <QLabel>
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
    mStatusLabel = new QLabel("");
    //
    QGridLayout *layout = new QGridLayout(this);
    layout->addWidget(mGraphWidget, 1, 0);
    layout->addWidget(mStatusLabel);
    setLayout(layout);
    //
    connect(
        this,
        SIGNAL(sigStatusChange(StatusKind, QString)),
        this,
        SLOT(mOnStatusChange(StatusKind, QString))
    );
    //
    mSetupMatrix();
}

void
MainFrame::mResetView(void)
{
    mZoomValue = sInitZoomValue;
    mSetupMatrix();
    // TODO For streaming data, make sure that the right side is visible.
    //mGraphWidget->ensureVisible(QRectF(0, 0, 0, 0));
}

void
MainFrame::mSetupMatrix(void)
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
MainFrame::mPrint(void)
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
MainFrame::mParseLogFile(
    const QString &fileName
) {
    mLegionProfLogParser = new LegionProfLogParser(fileName);
    emit sigStatusChange(StatusKind::INFO, "Parsing Log File...");
    mLegionProfLogParser->parse();
}

void
MainFrame::mOnStatusChange(
    StatusKind status,
    QString statusStr
) {
    QString colorString;
    switch (status) {
        case StatusKind::WARN:
        case StatusKind::ERR:
            colorString = "<font color='red'>";
            break;
        case StatusKind::INFO:
        default: break;
    }
    mStatusLabel->setText(colorString + statusStr);
}

void
MainFrame::mOnParseDone(
    void
) {
    const Status parseStatus = mLegionProfLogParser->status();
    if (parseStatus == Status::Okay()) {
        emit sigStatusChange(StatusKind::INFO, "Plotting");
        mGraphWidget->plot(mLegionProfLogParser->results());
        emit sigStatusChange(StatusKind::INFO, "");
    }
    else {
        emit sigStatusChange(StatusKind::ERR, parseStatus.errs);
    }
}

QString
MainFrame::mOpenLogFile(void)
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
        const QString fileName = mOpenLogFile();
        // TODO also check if we need to cleanup old plot.
        if (!fileName.isEmpty()) {
            QFuture<void> future = QtConcurrent::run(
                this,
                &MainFrame::mParseLogFile, fileName
            );
            QFutureWatcher<void> *watcher = new QFutureWatcher<void>();
            watcher->setFuture(future);
            connect(
                watcher,
                SIGNAL(finished()),
                this,
                SLOT(mOnParseDone(void))
            );
            connect(
                watcher,
                SIGNAL(finished()),
                watcher,
                SLOT(deleteLater())
            );
        }
        // Done in either case.
        return;
    }
    // Zoom In
    else if (keyEvent->matches(QKeySequence::ZoomIn) ||
             (commandPressed && keyEvent->key() == Qt::Key_Equal)) {
        if (mZoomValue < sMaxZoomValue) {
            mZoomValue += sZoomKeyIncrement;
            mSetupMatrix();
        }
        return;
    }
    // Zoom Out
    else if (keyEvent->matches(QKeySequence::ZoomOut)) {
        if (mZoomValue > sMinZoomValue) {
            mZoomValue -= sZoomKeyIncrement;
            mSetupMatrix();
        }
        return;
    }
    // Print
    else if (keyEvent->matches(QKeySequence::Print)) {
        mPrint();
        return;
    }
    switch (keyEvent->key()) {
        // Reset Zoom
        case Qt::Key_Equal: {
            mResetView();
            break;
        }
        default: QFrame::keyPressEvent(keyEvent);
    }
}
