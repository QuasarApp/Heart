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

DBCacheKey::DBCacheKey(AbstractKey * ptr) {
    setValue(ptr);
}

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

QString DBCacheKey::table() const {
    if (_value)
        return _value->table();

    return "";
}

DbId DBCacheKey::id() const {
    if (_value)
        return _value->id();

    return {};
}

uint hash(const DBCacheKey &key) {
    return key.value()->hash();
}

}
