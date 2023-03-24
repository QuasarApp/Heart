/*
 * Copyright (C) 2018-2023 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
*/

#include "dbobjectset.h"
namespace QH {
namespace PKG {
DBObjectSet::DBObjectSet(const QString table) {
    _table = table;
}

PrepareResult DBObjectSet::prepareInsertQuery(QSqlQuery &, bool ) const {
    return PrepareResult::Disabled;
}

PrepareResult DBObjectSet::prepareRemoveQuery(QSqlQuery &q) const {
    return DBObject::prepareRemoveQuery(q);
}

PrepareResult DBObjectSet::prepareSelectQuery(QSqlQuery &q) const {
    return DBObject::prepareSelectQuery(q);
}

PrepareResult DBObjectSet::prepareUpdateQuery(QSqlQuery &) const {
    return PrepareResult::Disabled;
}

bool DBObjectSet::isCached() const {
    return false;
}

bool DBObjectSet::isBundle() const {
    return true;
}

QString DBObjectSet::primaryKey() const {
    return "";
}

QString DBObjectSet::table() const {
    return _table;
}


}
}
