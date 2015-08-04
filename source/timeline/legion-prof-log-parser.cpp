/**
 * Copyright (c) 2015      Los Alamos National Security, LLC
 *                         All rights reserved.
 *
 * This file is part of the Gladius project. See the LICENSE.txt file at the
 * top-level directory of this distribution.
 */

#include "legion-prof-log-parser.h"

#include <QFile>
#include <QByteArray>
#include <QDebug>
#include <QRegExp>
#include <QtGlobal>
#include <QString>

#include <deque>

namespace {
/**
 * @brief taskInfoRx
 */
QRegExp gTaskInfoRx(
    "Prof Task Info ([0-9]+) ([0-9]+) "
    "([0-9]+) ([0-9]+) ([0-9]+) ([0-9]+) ([0-9]+)"
);

/**
 * @brief procDescRx
 */
QRegExp gProcDescRx(
    "Prof Proc Desc ([0-9]+) ([0-9]+)"
);

} // end namespace

/**
 * @brief LegionProfLogParser::LegionProfLogParser
 */
LegionProfLogParser::LegionProfLogParser(void)
{
    mProfData = nullptr;
}

/**
 * @brief LegionProfLogParser::~LegionProfLogParser
 */
LegionProfLogParser::~LegionProfLogParser(void)
{
    if (mProfData) {
        delete mProfData;
        mProfData = nullptr;
    }
}

/**
 * @brief LegionProfLogParser::parse
 * @param file
 */
void
LegionProfLogParser::parse(
    const QString &file
) {
    if (mProfData) delete mProfData;
    mProfData = new LegionProfData();
    //
    QFile inputFile(file);
    if (!inputFile.exists()) {
        //FIXME
        Q_ASSERT_X(false, __FILE__, "File Does Not Exist...");
    }
    if (!inputFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        //FIXME
        Q_ASSERT_X(false, __FILE__, "Cannot Open File...");
    }
    //
    while (!inputFile.atEnd()) {
        QString line(inputFile.readLine());
        //
        if (gTaskInfoRx.indexIn(line) != -1) {
            mProfData->taskInfos.push_back(
                TaskInfo(gTaskInfoRx.cap(1).toUInt(),
                         gTaskInfoRx.cap(2).toUInt(),
                         gTaskInfoRx.cap(3).toULongLong(),
                         gTaskInfoRx.cap(4).toULongLong(),
                         gTaskInfoRx.cap(5).toULongLong(),
                         gTaskInfoRx.cap(6).toULongLong(),
                         gTaskInfoRx.cap(7).toULongLong()
                )
            );
            continue;
        }
        if (gProcDescRx.indexIn(line) != -1) {
            mProfData->procDescs.push_back(
                ProcDesc(gProcDescRx.cap(1).toULongLong(),
                         gProcDescRx.cap(2).toUInt()
                )
            );
            continue;
        }
    }
    inputFile.close();

    qDebug() << "# Procs" << mProfData->procDescs.size();
    qDebug() << "Found " <<  mProfData->taskInfos.size();
}

/**
 * @brief LegionProfLogParser::parseSuccessful
 * @return
 */
bool
LegionProfLogParser::parseSuccessful(void) const
{
    if (!mProfData) return false;
    if (mProfData->procDescs.size() == 0) return false;
    return true;
}
