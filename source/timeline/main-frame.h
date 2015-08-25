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
#include <QPixmap>
#include <QStackedLayout>
#include <QStringList>
#include <QMap>
#include <QString>

QT_BEGIN_NAMESPACE
class QThreadPool;
class QLabel;
class QSlider;
class QToolButton;
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
    QThreadPool *mThreadPool = nullptr;
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
    QPixmap *mTimelinePixmap = nullptr;
    //
    QPixmap *mStatsPixmap = nullptr;
    //
    QToolButton *mGraphStatsButton = nullptr;
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
    bool
    mTimelineInFocus(void) {
        const auto cIndex = mStackedGraphStatsLayout->currentIndex();
        return StackedLayoutIndex::TIMELINE == cIndex;
    }
    //
    QPixmap *
    mGetGraphStatsButtonPixmap(bool pressed) {
        return (pressed ? mTimelinePixmap : mStatsPixmap);
    }
    //
    QString
    mGetGraphStatsButtonToolTip(bool pressed) {
        static const QString timelineTip = "Show Exection Timeline";
        static const QString statsTip    = "Show Statistics";
        return (pressed ? timelineTip : statsTip);
    }
};

#endif // TIMELINE_MAIN_FRAME_H_INCLUDED
