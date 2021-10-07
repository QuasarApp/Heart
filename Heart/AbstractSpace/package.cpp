/*
 * Copyright (C) 2018-2021 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
*/

#include "abstractdata.h"
#include "package.h"

#include <QDataStream>

namespace QH {

Package::Package() {
    Package::reset();
}

bool Package::isValid() const {
    if (!hdr.isValid()) {
        return false;
    }

    if (hdr.size != data.size()) {
        return false;
    }

    if (hdr.size > maximumSize())
        return false;

    return calcHash() == hdr.hash;
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

unsigned int Package::calcHash() const{
    return qHash(data + QByteArray::number(hdr.command));
}

int Package::maximumSize() {
    return 1400;
}

QDataStream &Package::fromStream(QDataStream &stream) {
    reset();
    stream.readRawData(reinterpret_cast<char*>(&hdr), sizeof(Header));

    char * buf = static_cast<char*>(malloc(hdr.size));
    stream.readRawData(buf, hdr.size);
    data.clear();
    data.insert(0, buf, hdr.size);

    free(buf);

    return stream;
}

QDataStream &Package::toStream(QDataStream &stream) const {
    stream.writeRawData(reinterpret_cast<const char*>(&hdr),
                        sizeof (hdr));

    stream.writeRawData(data.data(),
                        data.size());

    return stream;
}

}
