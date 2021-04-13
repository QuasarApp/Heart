/*
 * Copyright (C) 2018-2021 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
*/


#include "hostaddress.h"
#include <QDataStream>
#include <quasarapp.h>
namespace QH {

HostAddress::HostAddress() {

}

HostAddress::HostAddress(const QHostAddress &other, int port):
    QHostAddress(other) {
    setPort(port);
}

HostAddress::HostAddress(const QString &other, int port):
    QHostAddress(other) {
    setPort(port);
}

HostAddress::HostAddress(const SpecialAddress &other, int port):
    QHostAddress(other) {
    setPort(port);
}

HostAddress::HostAddress(const HostAddress &other):
    QHostAddress(*static_cast<const HostAddress*>(&other)) {

    setPort(other.port());
}

unsigned short HostAddress::port() const {
    return _port;
}

void HostAddress::setPort(unsigned short port) {
    _port = port;
}

bool HostAddress::isIP(const QString &address) {
    return !QHostAddress{address}.isNull();
}

bool HostAddress::isValid() const {
    return !isNull() && port();
}

QString HostAddress::toString() const {
    return QHostAddress::toString() + ":" + QString::number(port());
}

QDataStream &operator >>(QDataStream &stream, HostAddress &address) {
    stream >> static_cast<QHostAddress&>(address);
    stream >> address._port;

    return stream;
}

QDataStream &operator <<(QDataStream &stream, const HostAddress &address) {

    stream << static_cast<const QHostAddress&>(address);
    stream << address._port;

    return stream;
}

#define base(obj) static_cast<const QHostAddress*>(&obj)
bool operator ==(const HostAddress &left, const HostAddress &right) {
    return base(left)->operator==(*base(right)) && left.port() == right.port();
}

bool operator !=(const HostAddress &left, const HostAddress &right) {
    return base(left)->operator!=(*base(right)) && left.port() != right.port();
}

uint qHash(const HostAddress &address) {
    return qHash(QString("%1:%2").arg(address.toString()).arg(address.port()));
}

}
