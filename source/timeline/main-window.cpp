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
#include "legion-prof-log-parser.h"

#include <QtGlobal>
#include <QHBoxLayout>
#include <QGuiApplication>
#include <QScreen>

namespace {

/**
 * @brief getScreenGeometry
 * @param screenID
 * @return
 */
QRect
getScreenGeometry(
    unsigned screenID
) {
    return QGuiApplication::screens().at(screenID)->availableGeometry();
}

} // end namespace

/**
 * @brief MainWindow::MainWindow
 * @param parent
 */
MainWindow::MainWindow(
    QWidget *parent
) : QWidget(parent)
{
    // TODO add input name, job info, etc. to title?
    setWindowTitle(tr("Task Execution Timeline"));
    //
    static const int sScreenID = 0;
    QRect screenGeometry = getScreenGeometry(sScreenID);
    resize(screenGeometry.width(), screenGeometry.height() / 2);
    // TODO
    LegionProfLogParser logParser;
    logParser.parse("/Users/samuel/OUT.prof");
    if (!logParser.parseSuccessful()) {
         Q_ASSERT_X(false, __FILE__, "LegionProf Log Parse Failed...");
    }
    //
    mScene = new QGraphicsScene();
    mScene->addWidget(new TimelineWidget(logParser.results()));
    //
    View *view = new View("");
    view->view()->setScene(mScene);
    // Horizontal layout.
    QHBoxLayout *layout = new QHBoxLayout();
    layout->addWidget(view);
    //
    setLayout(layout);
}
