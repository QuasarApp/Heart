#include "dbaddress.h"
#include "permisiondata.h"

#include <QDataStream>

namespace NP {

bool operator ==(const PermisionData &left, const PermisionData &right) {
    return left._id == right._id && left._address == right._address;
}

unsigned int PermisionData::hash() const {
    QByteArray data;
    QDataStream stream(&data, QIODevice::WriteOnly);

    stream << _id;
    stream << _address;
    return qHash(data);
}

DbAddress PermisionData::address() const {
    return _address;
}

void PermisionData::setAddress(const DbAddress &address) {
    _address = address;
}

DbId PermisionData::id() const {
    return _id;
}

void PermisionData::setId(const DbId &Id) {
    _id = Id;
}

}
