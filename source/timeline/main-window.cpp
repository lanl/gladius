/**
 * Copyright (c) 2015      Los Alamos National Security, LLC
 *                         All rights reserved.
 *
 * This file is part of the Gladius project. See the LICENSE.txt file at the
 * top-level directory of this distribution.
 */

#include "main-window.h"
#include "view.h"
#include "timeline-widget.h"

#include <QHBoxLayout>
#include <QLineF>

MainWindow::MainWindow(
    QWidget *parent
) : QWidget(parent)
{
    // TODO add input name, job info, etc. to title?
    setWindowTitle(tr("Task Execution Timeline"));
    // FIXME Grab screen info
    resize(1000, 400);
    //
    populateScene();
    //
    View *view = new View("Top left view");
    view->view()->setScene(mScene);
    // Horizontal layout.
    QHBoxLayout *layout = new QHBoxLayout();
    layout->addWidget(view);
    //
    setLayout(layout);
}

/**
 * @brief The LegionProfReader class
 */
class LegionProfReader {
public:
    //
    LegionProfReader(void) {

    }
    //
};


/**
 * @brief MainWindow::populateScene
 */
void
MainWindow::populateScene(void)
{
    mScene = new QGraphicsScene();
    mScene->addWidget(new TimelineWidget());
#if 0
    //
    QRegExp taskInfoRx(
        "(Prof Task Info) ([0-9]+) ([0-9]+) "
        "([0-9]+) ([0-9]+) ([0-9]+) ([0-9]+) ([0-9]+)"
    );
    std::deque<TaskInfo> taskInfos;
    //
    QFile inputFile("/Users/samuel/OUT.prof");
    if (!inputFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "Cannot Open File!";
    }
    while (!inputFile.atEnd()) {
        QString line(inputFile.readLine());
        if (taskInfoRx.indexIn(line) != -1) {
            taskInfos.push_back(
                TaskInfo(taskInfoRx.cap(2).toUInt(),
                         taskInfoRx.cap(3).toUInt(),
                         taskInfoRx.cap(4).toULongLong(),
                         taskInfoRx.cap(5).toULongLong(),
                         taskInfoRx.cap(6).toULongLong(),
                         taskInfoRx.cap(7).toULongLong(),
                         taskInfoRx.cap(8).toULongLong()
                )
            );
        }
    }
    qDebug() << "Found " << taskInfos.size();
    inputFile.close();
    //
    qreal x = 0;
    qreal y = 0;
    qreal height = 32;
    static const uint32_t US_PER_PIXEL = 5000;

    QRectF testr(0, 0, 20, 20);


    for (const auto &taskInfo : taskInfos) {
        qreal width = (taskInfo.uStopTime - taskInfo.uStartTime) / US_PER_PIXEL;
        QRectF execRect(x, 0, width, height);
        x += width + 2;
        mScene->addRect(execRect);
    }
#endif
}
