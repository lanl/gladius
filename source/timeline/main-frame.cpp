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
        SIGNAL(sigStatusChange(QString)),
        this,
        SLOT(mOnStatusChange(QString))
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
MainFrame::mStartPlotFromLogFileThread(
    const QString &fileName
) {
    //
    emit sigStatusChange("Processing Log File...");
    QThread *thread = new QThread();
    mLegionProfLogParser = new LegionProfLogParser(fileName);
    mLegionProfLogParser->moveToThread(thread);
    //
    connect(
        thread,
        SIGNAL(started()),
        mLegionProfLogParser,
        SLOT(parse())
    );
    //
    connect(
        mLegionProfLogParser,
        SIGNAL(sigParseDone(bool, QString)),
        this,
        SLOT(mParseDone(bool, QString))
    );
    // QThread cleanup.
    connect(
        mLegionProfLogParser,
        SIGNAL(sigParseDone(bool, QString)),
        thread,
        SLOT(quit())
    );
    connect(
        thread,
        SIGNAL(finished()),
        thread,
        SLOT(deleteLater())
    );
    thread->start();
}

void
MainFrame::mParseDone(
    bool successful,
    QString status
) {
    if (successful) {
        mGraphWidget->plot(mLegionProfLogParser->results());
    }
    else {
        // TODO
        qDebug() << "Says Parse Is a Bad: " << status;
    }
    mLegionProfLogParser->deleteLater();
    emit sigStatusChange("");
}

void
MainFrame::mOnStatusChange(
    QString status
) {
    mStatusLabel->setText(status);
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
            mStartPlotFromLogFileThread(fileName);
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
