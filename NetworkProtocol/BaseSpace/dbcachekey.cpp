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

DBCacheKey::DBCacheKey(DBCacheKey &&other) {
    this->_value = other._value;
    other._value = nullptr;
}

DBCacheKey::DBCacheKey(const AbstractKey *ptr, bool fOnlyWraper) {
    setValue(ptr, fOnlyWraper);
}

DBCacheKey::~DBCacheKey() {
    if (_value && !_onlyWraper)
        delete _value;
}

const AbstractKey *DBCacheKey::value() const {
    return _value;
}

void DBCacheKey::setValue(const AbstractKey *value, bool fOnlyWraper) {
    _value = value;
    _onlyWraper = fOnlyWraper;
}

QString DBCacheKey::table() const {
    if (_value)
        return _value->table();

    return "";
}

BaseId DBCacheKey::id() const {
    if (_value)
        return _value->id();

    return {};
}

QString DBCacheKey::toString() const {
    return QString("table:%0 id:%1").arg(id().toBase64(), table());
}

bool DBCacheKey::isValid() const {
    return _value && _value->isValid();
}

bool operator ==(const DBCacheKey &left, const DBCacheKey &right) {
    if (!left._value)
        return left._value == right._value;
    return left._value->equal(right._value);
}

bool operator !=(const DBCacheKey &left, const DBCacheKey &right) {
    return !operator==(left, right);
}

uint qHash(const NP::DBCacheKey &key) {
    return key.value()->hash();
}

}

