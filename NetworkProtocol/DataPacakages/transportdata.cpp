#include "transportdata.h"
namespace NetworkProtocol {

TransportData::TransportData() {

}

TransportData::TransportData(const Package &package):AbstractData(package) {

}

const Package &TransportData::data() const {
    return _data;
}

void TransportData::setData(const Package &data) {
    _data = data;
}

QHostAddress TransportData::address() const {
    return _address;
}

void TransportData::setAddress(const QHostAddress &address) {
    _address = address;
}

QDataStream &TransportData::fromStream(QDataStream &stream) {
    stream >> _address;
    QByteArray array;
    stream >> array;
    _data.fromBytes(array);

    return stream;
}

QDataStream &TransportData::toStream(QDataStream &stream) const {
    stream << _address;
    stream << _data.toBytes();

    return stream;
}

bool TransportData::isValid() const {
    return !_address.isNull() && _data.isValid();
}
}
