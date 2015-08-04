/**
 * Copyright (c) 2015      Los Alamos National Security, LLC
 *                         All rights reserved.
 *
 * This file is part of the Gladius project. See the LICENSE.txt file at the
 * top-level directory of this distribution.
 */

#include "common.h"
#include "main-window.h"
#include "main-frame.h"

#include <QtGlobal>
#include <QHBoxLayout>
#include <QGuiApplication>
#include <QScreen>
#include <QString>

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
    setWindowTitle(tr(APP_WIN_TITLE));
    //
    static const int sScreenID = 0;
    QRect screenGeometry = getScreenGeometry(sScreenID);
    resize(screenGeometry.width(), screenGeometry.height() / 2);
    //
    MainFrame *mainFrame = new MainFrame(this);
    // Horizontal layout.
    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->addWidget(mainFrame);
    //
    setLayout(layout);
}
