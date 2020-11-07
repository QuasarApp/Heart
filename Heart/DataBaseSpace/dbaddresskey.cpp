/*
 * Copyright (C) 2018-2020 QuasarApp.
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

DbAddressKey::DbAddressKey(const QString &address, const BaseId &id):
   DbAddress(address, id) {

}

unsigned int DbAddressKey::hash() const {
    return qHash(*static_cast<const DbAddress*>(this));
}

const BaseId *DbAddressKey::id() const {
    return &DbAddress::id();
}

const QString *DbAddressKey::table() const {
    return &DbAddress::table();
}

bool DbAddressKey::isValid() const {
    return DbAddress::isValid();
}
}
