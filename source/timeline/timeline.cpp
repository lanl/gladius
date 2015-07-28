/**
 * Copyright (c) 2015      Los Alamos National Security, LLC
 *                         All rights reserved.
 *
 * This file is part of the Gladius project. See the LICENSE.txt file at the
 * top-level directory of this distribution.
 */

#include <QtCore>
#include <QScreen>
#include <QApplication>
#include <QtWidgets>
#include <QDebug>
#include <QLine>

/**
 *
 */
QRect
getScreenGeometry(
    unsigned screenID
) {
    return QGuiApplication::screens().at(screenID)->availableGeometry();
}

QBrush
getBackgroundColor(void)
{
    // #282829
    return QBrush(QColor(40, 40, 41));
}

/**
 *
 */
int
main(
    int argc,
    char **argv
) {
    qDebug() << "Starting Application...";
    //
    QApplication app(argc, argv);
    //
    QRect screenGeometry = getScreenGeometry(0);
    //
    qreal windowWidth = screenGeometry.width();
    qreal windowHeight = screenGeometry.height() / 2;
    //
    QGraphicsScene scene(0, 0, windowWidth, windowHeight);
    scene.setBackgroundBrush(getBackgroundColor());
    //
    QGraphicsView window(&scene);
    window.resize(windowWidth, windowHeight);
    window.show();
    //
    return app.exec();
}
