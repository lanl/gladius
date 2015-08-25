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

QRect
getScreenGeometry(
    unsigned screenID
) {
    return QGuiApplication::screens().at(screenID)->availableGeometry();
}

} // end namespace

MainWindow::MainWindow(
    QWidget *parent
) : QWidget(parent)
{
    // TODO add input name, job info, etc. to title?
    setWindowTitle(tr(APP_WIN_TITLE));
    //
    setStyleSheet(
        "QFrame {border: none; margin: 0px; padding: 0px;}"
        "QToolButton {border: none; margin: 0px; padding: 0px;}"
        "QToolButton:pressed { border: none; }"
        "QToolButton:checked { border: none; }"
    );
    //
    static const int sScreenID = 0;
    QRect screenGeometry = getScreenGeometry(sScreenID);
    resize(screenGeometry.width(), 2 * screenGeometry.height() / 3);
    //
    MainFrame *mainFrame = new MainFrame(this);
    // Horizontal layout.
    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->addWidget(mainFrame);
    //
    setLayout(layout);
}
