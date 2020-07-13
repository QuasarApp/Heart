#include "dbaddresskey.h"
namespace NP {

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

const BaseId &DbAddressKey::id() const {
    return DbAddress::id();
}

const QString &DbAddressKey::table() const {
    return DbAddress::table();
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
}
