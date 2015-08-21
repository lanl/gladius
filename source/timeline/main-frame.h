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
#include <QStringList>
#include <QMap>

QT_BEGIN_NAMESPACE
class QLabel;
class QSlider;
class QToolButton;
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

signals:
    void sigStatusChange(StatusKind kind, QString status);

private:
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
    GraphWidget *mGraphWidget = nullptr;
    // Map between log file name and parser.
    QMap<QString, LegionProfLogParser *> mLegionProfLogParsers;
    //
    QLabel *mStatusLabel = nullptr;
    //
    QStringList mOpenLogFiles(void);
    //
    void mParseLogFile(const QString &fileName);
    //
    void mProcessLogFiles(const QStringList &fileNames);
};

#endif // TIMELINE_MAIN_FRAME_H_INCLUDED
