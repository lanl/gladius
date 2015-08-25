/**
 * Copyright (c) 2015      Los Alamos National Security, LLC
 *                         All rights reserved.
 *
 * This file is part of the Gladius project. See the LICENSE.txt file at the
 * top-level directory of this distribution.
 */

#ifndef TIMELINE_MAIN_FRAME_H_INCLUDED
#define TIMELINE_MAIN_FRAME_H_INCLUDED

#include "info-types.h"

#include <QFrame>
#include <QStackedLayout>
#include <QStringList>
#include <QMap>

QT_BEGIN_NAMESPACE
class QLabel;
class QSlider;
class QPushButton;
class QTextEdit;
QT_END_NAMESPACE

class GraphWidget;
class LegionProfLogParser;

/**
 * @brief The View class
 */
class MainFrame : public QFrame {
    Q_OBJECT
public:
    //
    explicit MainFrame(QWidget *parent = nullptr);

protected:
    //
    void keyPressEvent(QKeyEvent *keyEvent) Q_DECL_OVERRIDE;

private slots:
    //
    void mResetView(void);
    //
    void mSetupMatrix(void);
    //
    void mPrint(void);
    //
    void mOnStatusChange(StatusKind status, QString statusStr);
    //
    void mOnParseDone(void);
    //
    void mOnGraphStatsButtonPressed(bool pressed);

signals:
    void sigStatusChange(StatusKind kind, QString status);

private:
    // Update if the order of mStackedGraphStatsLayout additions changes.
    enum StackedLayoutIndex {
        TIMELINE = 0,
        STATS
    };
    //
    static constexpr int sMinZoomValue  = 0;
    //
    static constexpr int sMaxZoomValue  = 512;
    //
    static constexpr int sInitZoomValue = sMaxZoomValue / 2;
    //
    static constexpr int sZoomKeyIncrement = 8;
    //
    int mZoomValue = 0;
    //
    int mNumFilesParsed = 0;
    //
    QStackedLayout *mStackedGraphStatsLayout = nullptr;
    //
    GraphWidget *mGraphWidget = nullptr;
    // Map between log file name and parser.
    QMap<QString, LegionProfLogParser *> mLegionProfLogParsers;
    //
    QLabel *mStatusLabel = nullptr;
    //
    QPushButton *mGraphStatsButton = nullptr;
    //
    QTextEdit *mStatsTextArea = nullptr;
    //
    QStringList mOpenLogFiles(void);
    //
    void mParseLogFile(const QString &fileName);
    //
    void mPreProcessLogFiles(void);
    //
    void mProcessLogFiles(const QStringList &fileNames);
    //
    bool mTimelineInFocus(void) {
        const auto cIndex = mStackedGraphStatsLayout->currentIndex();
        return StackedLayoutIndex::TIMELINE == cIndex;
    }
};

#endif // TIMELINE_MAIN_FRAME_H_INCLUDED
