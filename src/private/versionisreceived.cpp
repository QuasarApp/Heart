//#
//# Copyright (C) 2020-2024 QuasarApp.
//# Distributed under the lgplv3 software license, see the accompanying
//# Everyone is permitted to copy and distribute verbatim copies
//# of this license document, but changing it is not allowed.
//#

#include "versionisreceived.h"
namespace QH {
namespace PKG {

VersionIsReceived::VersionIsReceived()
{

}

QDataStream &VersionIsReceived::fromStream(QDataStream &stream) {
    return stream;
}

QDataStream &VersionIsReceived::toStream(QDataStream &stream) const {
    return stream;
}
}
}
