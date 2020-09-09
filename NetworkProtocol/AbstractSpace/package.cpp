/*
 * Copyright (C) 2018-2020 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
*/

#include "abstractdata.h"
#include "package.h"

#include <QDataStream>

namespace NP {

Package::Package() {
    reset();
}

bool Package::isValid() const {
    if (!hdr.isValid()) {
        return false;
    }

    auto rawint = data.mid(0, sizeof (decltype (hdr.command)));
    decltype (hdr.command) cmd;
    memcpy(&cmd, rawint.data(), sizeof (cmd));

    if (data.size() && hdr.command != cmd) {
        std::reverse(rawint.begin(), rawint.end());
        memcpy(&cmd, rawint.data(), sizeof (cmd));

        if (hdr.command != cmd)
            return false;
    }

    return hdr.size == data.size();
}

void Package::reset() {
    hdr.reset();
    data.clear();
}

QString Package::toString() const {
    return QString("Pakcage description: %0."
                   " Data description: Data size - %1, Data: %2").
            arg(hdr.toString()).arg(data.size()).arg(QString(data.toHex().toUpper()));
}

QDataStream &Package::fromStream(QDataStream &stream) {
    reset();

    char *hdr =  reinterpret_cast<char*>(&this->hdr);
    unsigned int readedBytes = sizeof(Header);
    stream.readBytes(hdr, readedBytes);

    return stream;
}

QDataStream &Package::toStream(QDataStream &stream) const {
    stream.writeBytes(reinterpret_cast<char*>(const_cast<Header*>(&hdr)),
                      sizeof (hdr));

    return stream;
}

}
