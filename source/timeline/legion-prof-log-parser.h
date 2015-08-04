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

#include <QString>
#include <deque>

/**
 * @brief The LegionProfLogParser class
 */
class LegionProfLogParser {
public:
    LegionProfLogParser(void);
    //
    ~LegionProfLogParser(void);
    // No copy constructor.
    LegionProfLogParser(const LegionProfLogParser&) = delete;
    // No assignment.
    LegionProfLogParser& operator=(const LegionProfLogParser&) = delete;
    //
    void parse(const QString &file);
    //
    bool parseSuccessful(void) const;
    /**
     * @brief results
     * @return
     */
    const LegionProfData& results(void) const {
        return *mProfData;
    }

private:
    LegionProfData *mProfData = nullptr;
};

#endif // TIMELINE_LEGIONPROFLOGPARSER_H
