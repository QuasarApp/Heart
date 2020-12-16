/*
 * Copyright (C) 2018-2020 QuasarApp.
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

AsyncSqlDbWriter::AsyncSqlDbWriter(QObject *ptr):
    SqlDBWriter(ptr) {

     _own = new QThread();
     _own->setObjectName("AsyncSqlDbWriter");
    moveToThread(_own);
    _own->start();

}

AsyncSqlDbWriter::~AsyncSqlDbWriter() {
    _own->quit();
    _own->wait();

    _own->deleteLater();
}
}
