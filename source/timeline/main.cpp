/**
 * Copyright (c) 2015      Los Alamos National Security, LLC
 *                         All rights reserved.
 *
 * This file is part of the Gladius project. See the LICENSE.txt file at the
 * top-level directory of this distribution.
 */


#include "common.h"
#include "main-window.h"

#include <QApplication>
#include <QDebug>

int
main(
    int argc,
    char **argv
) {
    Common::registerMetaTypes();
    //
    QApplication app(argc, argv);
    //
    app.setApplicationName(APP_NAME);
    //
    MainWindow window;
    window.show();
    //
    return app.exec();
}
