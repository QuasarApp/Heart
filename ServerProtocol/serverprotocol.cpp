#include "serverprotocol.h"

#include <QDataStream>
#include <QByteArray>
#include <QVariantMap>


ServerProtocol::Header::Header() {
    reset();
}

bool ServerProtocol::Header::isValid() const {

    if (sizeof (*this) != 2) {
        return false;
    }

    if (type == Type::Request) {
        return size < 256;
    }

    return size >= 5;
}

void ServerProtocol::Header::reset() {
    size = 0;
    command = Undefined;
    type = Responke;
}

ServerProtocol::Package::Package() {
    reset();
}

bool ServerProtocol::Package::isValid() const {
    if (!hdr.isValid()) {
        return false;
    }

    return hdr.size == data.size();
}

QVariantMap ServerProtocol::Package::parse() const {
    if (!isValid())
        return QVariantMap();

    QVariantMap res;
    QDataStream stream(data);

    res["command"] = hdr.command;
    res["type"] = hdr.type;

    if (hdr.type == Responke) {
        stream >> res;
    }

    return res;
}

QByteArray ServerProtocol::Package::toBytes() const {
    QByteArray res;
    res.append(reinterpret_cast<char*>(const_cast<Header*>(&hdr)),
               sizeof (hdr));

    res.append(data);
    return res;
}

void ServerProtocol::Package::reset() {
    hdr.reset();
    data.clear();
}

bool ServerProtocol::Package::fromMap(const QVariantMap &map) {
    QDataStream stream(&data, QIODevice::ReadWrite);

    stream << map;

    if (data.size() > 1024) {
        return false;
    }

    hdr.size =  static_cast<unsigned short>(data.size());

    return true;

}

