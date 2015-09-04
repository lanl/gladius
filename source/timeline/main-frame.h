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
#include <QStringList>

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
    void
    mFitViewToScene(void);
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
    //
    void mOnHelpButtonPressed(bool pressed);

signals:
    void sigStatusChange(StatusKind kind, QString status);

private:
    // Update if the order of mStackedGraphStatsLayout additions changes.
    enum StackedLayoutIndex {
        TIMELINE = 0,
        STATS,
        HELP
    };
    //
    static constexpr qreal sMinZoomValue  = -512.0;
    //
    static constexpr qreal sMaxZoomValue  = 512.0;
    //
    static constexpr qreal sZoomKeyIncrement = 4.0;
    //
    qreal mInitZoomValue = 0.0;
    //
    QThreadPool *mThreadPool = nullptr;
    //
    qreal mZoomValue = 0;
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
    QToolButton *mGraphStatsButton = nullptr;
    //
    QToolButton *mHelpButton = nullptr;
    //
    QTextEdit *mStatsTextArea = nullptr;
    //
    QTextEdit *mHelpTextArea = nullptr;
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
    QString
    mGetGraphStatsButtonToolTip(bool pressed) {
        static const QString timelineTip = "Show Exection Timeline";
        static const QString statsTip    = "Show Statistics";
        return (pressed ? timelineTip : statsTip);
    }
    //
    void mRecalibrateZoomValues(
        qreal targetScale
    );
    //
    QStringList
    mGetFileNamesFromArgv(void);
};

#endif // TIMELINE_MAIN_FRAME_H_INCLUDED
