/*
 * Copyright (C) 2018-2020 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
*/

#include "sqldb.h"
namespace QH {

SqlDB::SqlDB():
    ISqlDBCache(0, SqlDBCasheWriteMode::Force) {
}

void SqlDB::deleteFromCache(const QSharedPointer<PKG::DBObject> &delObj) {
    Q_UNUSED(delObj);
}

bool SqlDB::insertToCache(const QSharedPointer<PKG::DBObject> &obj) {
    Q_UNUSED(obj);
    return false;
}

bool SqlDB::updateCache(const QSharedPointer<PKG::DBObject> &obj) {
    Q_UNUSED(obj);
    return false;
}

QList<QSharedPointer<PKG::DBObject>>
SqlDB::getFromCache(const PKG::DBObject *obj) {
    Q_UNUSED(obj);
    return {};
}
}
