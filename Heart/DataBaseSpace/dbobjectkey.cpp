/*
 * Copyright (C) 2018-2020 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
*/


#include "dbobjectkey.h"

#include <dbobject.h>
namespace QH {


DBObjectKey::DBObjectKey(const PKG::DBObject *dbObjcet) {
    _source = dbObjcet;
}

unsigned int DBObjectKey::hash() const {
    if (!_source)
        return 0;

    return qHash(_source->getQuerySrting());
}

const BaseId *DBObjectKey::id() const {
    if (!_source)
        return nullptr;

    return &_source->getId();
}

const QString *DBObjectKey::table() const {
    if (!_source)
        return nullptr;

    return &_source->tableName();
}

bool DBObjectKey::isValid() const {
    if (!_source)
        return 0;

    return _source->isValid();
}

}
