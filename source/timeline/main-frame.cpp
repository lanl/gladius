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
#include <QFileDialog>
#include <QString>
#include <QLabel>
#include <QtConcurrent>
#include <QMutex>
#include <QThread>
#include <QtWidgets>
#include <QStringList>

#ifndef QT_NO_PRINTER
#include <QPrinter>
#include <QPrintDialog>
#endif

#include <cassert>

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
    static QMutex mutex;
    mutex.lock();
    auto *aParser = mLegionProfLogParsers[fileName];
    aParser->parse();
    mutex.unlock();
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
    static QMutex mutex;
    //
    mutex.lock();
    bool doPlot = false;
    mNumFilesParsed++;
    if (mNumFilesParsed == mLegionProfLogParsers.size()) {
        doPlot = true;
    }
    //
    if (!doPlot) {
        mutex.unlock();
        return;
    }
    mutex.unlock();
    // Else we try to plot...
    bool allGood = true;
    foreach (LegionProfLogParser *p, mLegionProfLogParsers) {
        const Status parseStatus = p->status();
        if (parseStatus != Status::Okay()) {
            emit sigStatusChange(StatusKind::ERR, parseStatus.errs);
            allGood = false;
            break;
        }
    }
    //
    if (!allGood) return;
    // It's all good, so plot the data.
    emit sigStatusChange(StatusKind::INFO, "Plotting...");
    foreach (LegionProfLogParser *p, mLegionProfLogParsers) {
        // FIXME consider adding a "addPlotData" to avoid some of the costly
        // operations in plot. Then at the end plot.
        mGraphWidget->plot(p->results());
        p->deleteLater();
        p = nullptr;
    }
    emit sigStatusChange(StatusKind::INFO, "");
}

QStringList
MainFrame::mOpenLogFiles(void)
{
    QStringList fileNames = QFileDialog::getOpenFileNames(
        this,
        tr("Open Log File"),
        QDir::homePath(),
        tr("Log Files (*.*)")
    );
    //
    return fileNames;
}

void
MainFrame::mPreProcessLogFiles()
{
    foreach(LegionProfLogParser *p, mLegionProfLogParsers) {
        if (p) p->deleteLater();
    }
    mLegionProfLogParsers.clear();
    mNumFilesParsed = 0;
}

void
MainFrame::mProcessLogFiles(
    const QStringList &fileNames
) {
    // TODO also check if we need to cleanup old plot.
    const auto numFiles = fileNames.size();
    emit sigStatusChange(
        StatusKind::INFO,
        "Processing " + QString::number(numFiles) +
        " Log File" + (numFiles > 1 ? "s" : "")
    );
    // Perform initial population of the parser map. This is done here so we
    // query the map for its size which will be used to determine if all the
    // parses are done.
    foreach (const QString fileName, fileNames) {
        assert(!mLegionProfLogParsers.contains(fileName));
        auto *aParser = new LegionProfLogParser(fileName);
        mLegionProfLogParsers[fileName] = aParser;
    }
    //
    foreach (const QString fileName, fileNames) {
        QFuture<void> future = QtConcurrent::run(
            this,
            &MainFrame::mParseLogFile, fileName
        );
        //
        auto *watcher = new QFutureWatcher<void>();
        watcher->setFuture(future);
        //
        connect(watcher, SIGNAL(finished()), this, SLOT(mOnParseDone()));
        //
        connect(watcher, SIGNAL(finished()), watcher, SLOT(deleteLater()));
    }
}

void
MainFrame::keyPressEvent(
    QKeyEvent *keyEvent
) {
    const bool commandPressed = (keyEvent->modifiers() & Qt::ControlModifier);
    // Open Log File
    if (keyEvent->matches(QKeySequence::Open)) {
        const QStringList fileNames = mOpenLogFiles();
        if (!fileNames.isEmpty()) {
            // Do cleanup if need be.
            mPreProcessLogFiles();
            mProcessLogFiles(fileNames);
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
