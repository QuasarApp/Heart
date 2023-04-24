/*
 * Copyright (C) 2023-2023 QuasarApp.
 * Distributed under the GPLv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
*/

#include "universaldata.h"


namespace QH {
namespace PKG {

UniversalData::UniversalData()
{

}

void UniversalData::setValue(int key, const QVariant &value) {
    _data[key] = value;
}

const QVariant &UniversalData::value(int key, const QVariant &defaultVal) const {

    auto nonConstPtr = const_cast<QHash<int, QVariant>*>(&_data);
    if (!nonConstPtr)
        return defaultVal;

    if (nonConstPtr->contains(key)) {
        return nonConstPtr->operator[](key);
    }

    return defaultVal;
}

QVariant *UniversalData::ref(int key) {

    if (_data.contains(key)) {
        return &_data.operator[](key);
    }

    return nullptr;
}

QDataStream &UniversalData::fromStream(QDataStream &stream) {
    stream >> _data;

    return stream;
}

QDataStream &UniversalData::toStream(QDataStream &stream) const {
    stream << _data;

    return stream;
}
}
}
