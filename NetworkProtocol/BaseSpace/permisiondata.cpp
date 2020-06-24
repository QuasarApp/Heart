#include "dbaddress.h"
#include "permisiondata.h"
#include <quasarapp.h>
#include <QDataStream>

namespace NP {

bool operator ==(const PermisionData &left, const PermisionData &right) {
    return left._id == right._id && left._address == right._address;
}

PermisionData::PermisionData(const DbId &subject, const DbAddress &objcet) {
    setId(subject);
    setAddress(objcet);
}

unsigned int PermisionData::hash() const {
    QByteArray data;
    QDataStream stream(&data, QIODevice::WriteOnly);

    stream << _id;
    stream << _address;
    return qHash(data);
}

const DbId &PermisionData::id() const {
    return _id;
}

const QString &PermisionData::table() const {
    return _address.table();
}

bool PermisionData::isValid() const {
    return address().isValid() && _id.isValid();
}

DbAddress PermisionData::address() const {
    return _address;
}

void PermisionData::setAddress(const DbAddress &address) {
    _address = address;
}

QDataStream &PermisionData::fromStream(QDataStream &stream) {
    stream >> _id;
    stream >> _address;

    return stream;
}

QDataStream &PermisionData::toStream(QDataStream &stream) const {
    stream << _id;
    stream << _address;

    return stream;
}

void PermisionData::setId(const DbId &Id) {
    _id = Id;
}

}
