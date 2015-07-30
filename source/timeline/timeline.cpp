/**
 * Copyright (c) 2015      Los Alamos National Security, LLC
 *                         All rights reserved.
 *
 * This file is part of the Gladius project. See the LICENSE.txt file at the
 * top-level directory of this distribution.
 */


#include "main-window.h"

#include <QApplication>
#include <QDebug>

/**
 * @brief main
 * @param argc
 * @param argv
 * @return
 */
int
main(
    int argc,
    char **argv
) {
    qDebug() << "Starting Application...";
    //
    QApplication app(argc, argv);
    app.setAttribute(Qt::AA_DontCreateNativeWidgetSiblings);
    //
    MainWindow window;
    window.show();
    //
    return app.exec();
}
