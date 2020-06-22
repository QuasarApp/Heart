/*
 * Copyright (C) 2018-2020 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
*/


#include "dbcachekey.h"

#include <abstractkey.cpp>
namespace NP {

DBCacheKey::DBCacheKey() {}

DBCacheKey::~DBCacheKey() {
    if (_value)
        delete _value;
}

AbstractKey *DBCacheKey::value() const {
    return _value;
}

void DBCacheKey::setValue(AbstractKey *value) {
    _value = value;
}

uint hash(const DBCacheKey &key) {
    return key.value()->hash();
}

}
