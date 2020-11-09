/*
 * Copyright (C) 2018-2020 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
*/

#include "dbobjectset.h"
namespace QH {
namespace PKG {
DBObjectSet::DBObjectSet(const QString table):
    DBObject(table, "") {

}

PrepareResult DBObjectSet::prepareSaveQuery(QSqlQuery &) const {
    return PrepareResult::Disabled;
}

PrepareResult DBObjectSet::prepareRemoveQuery(QSqlQuery &q) const {
    return DBObject::prepareRemoveQuery(q);
}

PrepareResult DBObjectSet::prepareSelectQuery(QSqlQuery &q) const {
    return DBObject::prepareSelectQuery(q);
}

bool DBObjectSet::isCached() const {
    return false;
}

bool DBObjectSet::isBundle() const {
    return true;
}

}
}
