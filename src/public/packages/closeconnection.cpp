/*
 * Copyright (C) 2018-2022 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
*/


#include "closeconnection.h"

namespace QH {
namespace PKG {

CloseConnection::CloseConnection() {

}

QDataStream &CloseConnection::fromStream(QDataStream &stream) {
    return stream;
}

QDataStream &CloseConnection::toStream(QDataStream &stream) const {
    return stream;
}
}
}
