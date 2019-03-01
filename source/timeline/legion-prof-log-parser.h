/**
 * Copyright (c) 2015      Triad National Security, LLC
 *                         All rights reserved.
 *
 * This file is part of the Gladius project. See the LICENSE.txt file at the
 * top-level directory of this distribution.
 */

#ifndef TIMELINE_LEGION_PROF_LOG_PARSER_H
#define TIMELINE_LEGION_PROF_LOG_PARSER_H

#include "common.h"
#include "info-types.h"

#include <QObject>
#include <deque>

QT_BEGIN_NAMESPACE
class QString;
QT_END_NAMESPACE

class LegionProfLogParser : public QObject {
    Q_OBJECT

public:
    LegionProfLogParser(QString file);
    //
    ~LegionProfLogParser(void);
    // No copy constructor.
    LegionProfLogParser(const LegionProfLogParser&) = delete;
    // No assignment.
    LegionProfLogParser& operator=(const LegionProfLogParser&) = delete;
    //
    const LegionProfData& results(void) const {
        return *mProfData;
    }
    //
    Status
    status(void) { return mStatus; }
    //
    QString
    getFileName(void) const { return mFileName; }

public slots:
    //
    void parse(void);

signals:
    void sigParseDone(void);

private:
    //
    Status mStatus;
    //
    QString mFileName;
    //
    LegionProfData *mProfData = nullptr;
    //
    bool mParseSuccessful(void) const;
};

#endif // TIMELINE_LEGION_PROF_LOG_PARSER_H
