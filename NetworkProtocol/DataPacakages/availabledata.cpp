/*
 * Copyright (C) 2018-2020 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
*/

#include "availabledata.h"

#include <QDataStream>

namespace NP {

AvailableData::AvailableData() {
    INIT_COMMAND
}

AvailableData::AvailableData(const Package &pkg):AvailableData() {
    fromBytes(pkg.toBytes());
}

QDataStream &AvailableData::fromStream(QDataStream &stream) {
    AbstractData::fromStream(stream);
    stream >> _data;
    return stream;
}

QDataStream &AvailableData::toStream(QDataStream &stream) const {
    AbstractData::toStream(stream);
    stream << _data;
    return stream;
}

bool AvailableData::isValid() const {
    return AbstractData::isValid() && _data.size();
}

QHash<QString, QList<int> > AvailableData::data() const {
    return _data;
}

void AvailableData::setData(const QHash<QString, QList<int> > &data) {
    _data = data;
}

QList<int>& AvailableData::operator[](const QString &key) {
    return _data[key];
}

}

