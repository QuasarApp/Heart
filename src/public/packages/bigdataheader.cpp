/*
 * Copyright (C) 2021-2024 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
*/

#include "bigdataheader.h"

namespace QH {
namespace PKG {

BigDataHeader::BigDataHeader() {

}

bool BigDataHeader::isValid() const {
    return BigDataBase::isValid() && packagesCount >= 2 && _command;
}

QString BigDataHeader::toString() const {
    return BigDataBase::toString() +
            " Packages count: " +
            QString::number(packagesCount) +
            " Packages command: " +
            QString::number(_command);
}

int BigDataHeader::getPackagesCount() const {
    return packagesCount;
}

void BigDataHeader::setPackagesCount(int newPackagesCount) {
    packagesCount = newPackagesCount;
}

QDataStream &BigDataHeader::fromStream(QDataStream &stream) {
    BigDataBase::fromStream(stream);

    stream >> packagesCount;
    stream >> _command;

    return stream;
}

QDataStream &BigDataHeader::toStream(QDataStream &stream) const {
    BigDataBase::toStream(stream);

    stream << packagesCount;
    stream << _command;

    return stream;
}

unsigned short BigDataHeader::getCommand() const {
    return _command;
}

void BigDataHeader::setCommand(unsigned short newCommand) {
    _command = newCommand;
}

}
}
