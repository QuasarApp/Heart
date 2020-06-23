#include "dbaddresskey.h"
namespace NP {

DbAddressKey::DbAddressKey() {

}

DbAddressKey::DbAddressKey(const DbAddress &address):
    DbAddress(address) {

}

DbAddressKey::DbAddressKey(const QString &address, const DbId &id):
   DbAddress(address, id) {

}

unsigned int DbAddressKey::hash() const {
    return qHash(*static_cast<const DbAddress*>(this));
}

const DbId &DbAddressKey::id() const {
    return DbAddress::id();
}

const QString &DbAddressKey::table() const {
    return DbAddress::table();
}
}