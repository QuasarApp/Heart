#include "user.h"

#include <QDataStream>
namespace QH {
namespace PKG {

User::User() {
    setTrust(100);
}

User::User(const Package &pkg):
    User() {
    fromBytes(pkg.data);
}

User::User(const BaseId &id):
    User() {
    setId(id);
}

bool User::copyFrom(const AbstractData *other) {
    if (!NetworkMember::copyFrom(other))
        return false;

    auto otherObject = dynamic_cast<const User*>(other);
    if (!otherObject)
        return false;

    this->_token = otherObject->_token;
    this->_type = otherObject->_type;

    return true;
}

bool User::fromSqlRecord(const QSqlRecord &q) {
    if (!NetworkMember::fromSqlRecord(q)) {
        return false;
    }

    setToken(AccessToken(q.value("token").toByteArray()));

    return User::isValid();
}

DBObject *User::createDBObject() const {
    return create<User>();
}

bool User::isValid() const {
    return NetworkMember::isValid() && trust() <= 100;
}

BaseId User::generateId() const {
    return getId();
}

QDataStream &User::fromStream(QDataStream &stream) {
    NetworkMember::fromStream(stream);
    stream >> _token;
    stream >> _type;

    return stream;
}

QDataStream &User::toStream(QDataStream &stream) const {
    NetworkMember::toStream(stream);
    stream << _token;
    stream << _type;

    return stream;
}

DBVariantMap User::variantMap() const {
    auto map = NetworkMember::variantMap();
    map.insert("token", {_token.toBytes(), MemberType::InsertUpdate});

    return map;
}

UserRequestType User::type() const {
    return _type;
}

void User::setType(const UserRequestType &type) {
    _type = type;
}

AccessToken User::token() const {
    return _token;
}

void User::setToken(const AccessToken &token) {
    _token = token;
}
}
}
