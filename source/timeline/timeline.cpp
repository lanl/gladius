/**
 * Copyright (c) 2015      Los Alamos National Security, LLC
 *                         All rights reserved.
 *
 * This file is part of the Gladius project. See the LICENSE.txt file at the
 * top-level directory of this distribution.
 */

#include <QApplication>
#include <QDebug>

/**
 *
 */
int
main(
    int argc,
    char **argv
) {
    QApplication app(argc, argv);
    qDebug() << "Hello World";
    return app.exec();
}
