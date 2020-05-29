/*
 * Copyright (C) 2018-2020 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
*/

#include "dbobjectsfactory.h"

#include <dbobject.h>

namespace NP {

DbObjectsFactory::DbObjectsFactory() {

}

SP<DBObject> DbObjectsFactory::factory(const QString & table) {
    return instance()->_data.value(table)->factory();
}

DbObjectsFactory *DbObjectsFactory::instance() {
    DbObjectsFactory * i = new DbObjectsFactory();
    return i;
}

bool DbObjectsFactory::registerObject(const QString & key, SP<DBObject> obj) {
    if (obj.isNull())
        return false;

    _data[key] = obj;
    return true;
}

}
