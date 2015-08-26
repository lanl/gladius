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
#include <QToolButton>
#include <QStackedLayout>
#include <QThreadPool>

#ifndef QT_NO_PRINTER
#include <QPrinter>
#include <QPrintDialog>
#endif

#include <cassert>
#include <cmath>

#include <qmath.h>

MainFrame::MainFrame(
    QWidget *parent
) : QFrame(parent)
{
    static const int maxThreads = 8;
    mThreadPool = new QThreadPool(this);
    mThreadPool->setMaxThreadCount(maxThreads);
    // Page 1
    mGraphWidget = new GraphWidget();
    // Page 2
    mStatsTextArea = new QTextEdit();
    mStatsTextArea->setReadOnly(true);
    //
    mStatusLabel = new QLabel(this);
    //
    // Icons From: http://google.github.io/material-design-icons/
    mTimelinePixmap = new QPixmap(":/icons/timeline.24px.svg");
    mStatsPixmap = new QPixmap(":/icons/stats.24px.svg");
    //
    mGraphStatsButton = new QToolButton(this);
    mGraphStatsButton->setCheckable(true);
    mGraphStatsButton->setIconSize(mTimelinePixmap->size());
    // Hide until we have things to show.
    mGraphStatsButton->hide();
    //
    QGridLayout *layout = new QGridLayout(this);
    // We stack the graph and the stats for a given graph window.
    mStackedGraphStatsLayout = new QStackedLayout();
    mStackedGraphStatsLayout->addWidget(mGraphWidget);
    mStackedGraphStatsLayout->addWidget(mStatsTextArea);
    //
    layout->addLayout(mStackedGraphStatsLayout, 0, 0);
    layout->addWidget(mGraphStatsButton,        1, 0, Qt::AlignLeft);
    layout->addWidget(mStatusLabel,             1, 0, Qt::AlignRight);
    setLayout(layout);
    //
    connect(
        this,
        SIGNAL(sigStatusChange(StatusKind, QString)),
        this,
        SLOT(mOnStatusChange(StatusKind, QString))
    );
    connect(
        mGraphStatsButton,
        SIGNAL(clicked(bool)),
        this,
        SLOT(mOnGraphStatsButtonPressed(bool))
    );
}

void
MainFrame::mRecalibrateZoomValues(qreal targetScale)
{
    // Solve 2^(x/50) = targetScale to get x. x is the target value.
    const qreal x = 50.0 * std::log2(targetScale);
    // Now solve: x = mZoomValue - sInitZoomValue to get mZoomValue
    mZoomValue = x + mInitZoomValue;
    //
    mSetupMatrix();
}

void
MainFrame::mSetupMatrix(void)
{
    const qreal scale = qPow(
        2.0,
        (mZoomValue - mInitZoomValue) / 50.0
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
    QString msgBegin;
    QString msgEnd = "]";
    switch (status) {
        case StatusKind::WARN:
            msgBegin = "Warning [";
            break;
        case StatusKind::ERR:
            msgBegin = "Error [";
            break;
        case StatusKind::INFO:
        default:
            msgBegin = "";
            msgEnd   = "";
            break;
    }
    mStatusLabel->clear();
    mStatusLabel->setText(msgBegin + statusStr + msgEnd);
}

void
MainFrame::mOnParseDone(
    void
) {
    static QMutex mutex;
    //
    mutex.lock(); //////////////////////////////////////////////////////////////
    bool doPlot = false;
    const auto totalNumFiles = mLegionProfLogParsers.size();
    mNumFilesParsed++;
    const float percentDone = (float(mNumFilesParsed) / totalNumFiles) * 100.0;
    //
    emit sigStatusChange(
        StatusKind::INFO,
        "Parsing: " + QString::number(percentDone, 'f', 0) + "% Done"
    );
    //
    if (mNumFilesParsed == totalNumFiles) {
        doPlot = true;
    }
    //
    if (!doPlot) {
        mutex.unlock(); ////////////////////////////////////////////////////////
        return;
    }
    mutex.unlock(); ////////////////////////////////////////////////////////////
    // Else we try to plot... (one thread only)
    bool allGood = true;
    foreach (LegionProfLogParser *p, mLegionProfLogParsers) {
        const Status parseStatus = p->status();
        if (parseStatus != Status::Okay()) {
            const QString errs = p->getFileName() + ": " + parseStatus.errs;
            emit sigStatusChange(StatusKind::ERR, errs);
            allGood = false;
            break;
        }
    }
    //
    if (!allGood) return;
    // It's all good, so plot the data.
    foreach (LegionProfLogParser *p, mLegionProfLogParsers) {
        mGraphWidget->addPlotData(p->results());
    }
    //
    mGraphWidget->plot();
    //
    mFitViewToScene();
    // We no longer need the parser instances, so clean them up.
    foreach (const QString fName, mLegionProfLogParsers.keys()) {
        mLegionProfLogParsers[fName]->deleteLater();
        mLegionProfLogParsers[fName] = nullptr;
    }
    // All done!
    emit sigStatusChange(StatusKind::INFO, "");
    // Now we can show this button.
    mOnGraphStatsButtonPressed(false);
    mGraphStatsButton->show();
}

void
MainFrame::mOnGraphStatsButtonPressed(
    bool pressed
) {
    const int panelIndex = (
        pressed ? StackedLayoutIndex::STATS : StackedLayoutIndex::TIMELINE
    );
    //
    mGraphStatsButton->setIcon(*mGetGraphStatsButtonPixmap(pressed));
    mGraphStatsButton->setToolTip(mGetGraphStatsButtonToolTip(pressed));
    mStackedGraphStatsLayout->setCurrentIndex(panelIndex);
}

QStringList
MainFrame::mOpenLogFiles(void)
{
    QStringList fileNames = QFileDialog::getOpenFileNames(
        this,
        tr("Open Log Files"),
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
            mThreadPool,
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
MainFrame::mFitViewToScene(void)
{
    const auto viewRect = mGraphWidget->viewport()->rect();
    const auto sceneRect = mGraphWidget->scene()->sceneRect();
    static const qreal fudge = 0.9;
    const qreal scale = (viewRect.height() * fudge) / sceneRect.height();
    // Now that we have the target scale now we need to update the zoom values
    // so that we have a nice transtion between the scaled view and user
    // zoom interactions.
    mRecalibrateZoomValues(scale);
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
        if (!mTimelineInFocus()) return;
        //
        if (mZoomValue < sMaxZoomValue) {
            mZoomValue += sZoomKeyIncrement;
            mSetupMatrix();
        }
        return;
    }
    // Zoom Out
    else if (keyEvent->matches(QKeySequence::ZoomOut)) {
        if (!mTimelineInFocus()) return;
        //
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
            if (!mTimelineInFocus()) return;
            //
            mFitViewToScene();
            break;
        }
        default: QFrame::keyPressEvent(keyEvent);
    }
}
