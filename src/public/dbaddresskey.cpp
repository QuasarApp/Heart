/*
 * Copyright (C) 2018-2025 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
*/


#include "dbaddresskey.h"
namespace QH {

DbAddressKey::DbAddressKey() {

}

DbAddressKey::DbAddressKey(const DbAddress &address):
    DbAddress(address) {

}

DbAddressKey::DbAddressKey(const QString &table, const QVariant &value):
   DbAddress(table, value) {

}

unsigned int DbAddressKey::hash() const {
    return qHash(*static_cast<const DbAddress*>(this));
}

bool DbAddressKey::equal(const AbstractKey *other) const {
    auto otherObject = dynamic_cast<const DbAddressKey*>(other);

    if (!otherObject)
        return false;

    return operator==(*static_cast<const DbAddress*>(this), *otherObject);
}

bool DbAddressKey::isValid() const {
    return DbAddress::isValid();
}

QString DbAddressKey::toString() const {
    return DbAddress::toString();
}
}
