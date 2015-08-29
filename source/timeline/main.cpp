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
#include <QStringList>
#include <QString>
#include <QDebug>
#include <QTextStream>


namespace {

bool needsHelp(
    const QStringList &argv
) {
    static const QString h0 = "-help";
    static const QString h1 = "--help";
    static const QString h2 = "-h";

    foreach (const QString &arg, argv) {
        if (arg == h0 || arg == h1 || arg == h2) {
            return true;
        }
    }
    return false;
}

void
displayUsage(void) {
    QTextStream(stdout) << "usage: " APP_NAME " [log ...]" << endl;
}

} // end namespace

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
    if (needsHelp(QCoreApplication::arguments())) {
        displayUsage();
        return EXIT_SUCCESS;
    }
    //
    MainWindow window;
    window.show();
    //
    return app.exec();
}
