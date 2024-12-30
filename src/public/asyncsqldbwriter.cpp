/*
 * Copyright (C) 2018-2025 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
*/

#include "asyncsqldbwriter.h"

#include <QDateTime>
#include <QMetaMethod>
#include <QThread>
#include <dbobject.h>
#include <quasarapp.h>

namespace QH {

using namespace PKG;

AsyncSqlDBWriter::AsyncSqlDBWriter(QObject *ptr):
    SqlDBWriter(new QThread(), ptr) {
    thread()->setObjectName("AsyncSqlDbWriter");
    thread()->start();
}

AsyncSqlDBWriter::~AsyncSqlDBWriter() {
    thread()->quit();
    thread()->wait();

    thread()->deleteLater();
}
}
