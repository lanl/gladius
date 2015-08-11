/**
 * Copyright (c) 2015      Los Alamos National Security, LLC
 *                         All rights reserved.
 *
 * This file is part of the Gladius project. See the LICENSE.txt file at the
 * top-level directory of this distribution.
 */

#include "common.h"
#include "legion-prof-log-parser.h"

#include <QFile>
#include <QByteArray>
#include <QDebug>
#include <QRegExp>
#include <QtGlobal>
#include <QString>

#include <deque>

namespace {
//
QRegExp gTaskInfoRx(
    "Prof Task Info ([0-9]+) ([0-9]+) "
    "([0-9]+) ([0-9]+) ([0-9]+) ([0-9]+) ([0-9]+)"
);

QRegExp gMetaInfoRx(
    "Prof Meta Info ([0-9]+) ([0-9]+) "
    "([0-9]+) ([0-9]+) ([0-9]+) ([0-9]+) ([0-9]+)"
);

// TODO (???) (???)
QRegExp gProcDescRx(
    "Prof Proc Desc ([0-9]+) ([0-9]+)"
);

// (Task ID) (Task Name)
QRegExp gTaskKindRx(
    "Prof Task Kind ([0-9]+) ([a-zA-Z0-9_]+)"
);

// (Operation ID) (Operation Name)
QRegExp gMetaDescRx(
    "Prof Meta Desc ([0-9]+) ([a-zA-Z0-9_]+)"
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
        if (gTaskKindRx.indexIn(line) != -1) {
            const taskid_t tid = gTaskKindRx.cap(1).toUInt();
            const std::string tname = gTaskKindRx.cap(2).toStdString();
            mProfData->taskKinds.insert(
                std::make_pair(tid, TaskKind(tid, tname))
            );
            continue;
        }
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
        if (gMetaInfoRx.indexIn(line) != -1) {
            mProfData->metaInfos.push_back(
                TaskInfo(gMetaInfoRx.cap(1).toUInt(),
                         gMetaInfoRx.cap(2).toUInt(),
                         gMetaInfoRx.cap(3).toULongLong(),
                         gMetaInfoRx.cap(4).toULongLong(),
                         gMetaInfoRx.cap(5).toULongLong(),
                         gMetaInfoRx.cap(6).toULongLong(),
                         gMetaInfoRx.cap(7).toULongLong()
                )
            );
            continue;
        }
        if (gProcDescRx.indexIn(line) != -1) {
            mProfData->procDescs.push_back(
                ProcDesc(gProcDescRx.cap(1).toULongLong(),
                         static_cast<ProcType>(gProcDescRx.cap(2).toUInt())
                )
            );
            continue;
        }
        if (gMetaDescRx.indexIn(line) != -1) {
            const opid_t opid = gMetaDescRx.cap(1).toUInt();
            const std::string opName = gMetaDescRx.cap(2).toStdString();
            mProfData->metaDescs.insert(
                std::make_pair(opid, MetaDesc(opid, opName))
            );
            continue;
        }
    }
    inputFile.close();

    qDebug() << "# Proc Kinds Found:" << mProfData->taskKinds.size();
    qDebug() << "# Procs Found:" << mProfData->procDescs.size();
    qDebug() << "# Task Infos Found:" <<  mProfData->taskInfos.size();
    qDebug() << "# Meta Descs Found:" <<  mProfData->metaDescs.size();
    qDebug() << "# Meta Infos Found:" <<  mProfData->metaInfos.size();
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
