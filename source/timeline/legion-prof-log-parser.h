/**
 * Copyright (c) 2015      Los Alamos National Security, LLC
 *                         All rights reserved.
 *
 * This file is part of the Gladius project. See the LICENSE.txt file at the
 * top-level directory of this distribution.
 */

#ifndef TIMELINE_LEGION_PROF_LOG_PARSER_H
#define TIMELINE_LEGION_PROF_LOG_PARSER_H

#include "info-types.h"

#include <QObject>
#include <QString>
#include <deque>

/**
 * @brief The LegionProfLogParser class
 */
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
    bool parseSuccessful(void) const;
    //
    const LegionProfData& results(void) const {
        return *mProfData;
    }

public slots:
    //
    void parse(void);

signals:
    void sigParseDone(
        bool successful,
        QString status
    );

private:
    //
    QString mFile;
    //
    LegionProfData *mProfData = nullptr;
};

#endif // TIMELINE_LEGION_PROF_LOG_PARSER_H
