//#
//# Copyright (C) 2020-2022 QuasarApp.
//# Distributed under the lgplv3 software license, see the accompanying
//# Everyone is permitted to copy and distribute verbatim copies
//# of this license document, but changing it is not allowed.
//#


#ifndef VERSIONISRECEIVED_H
#define VERSIONISRECEIVED_H

#include <abstractdata.h>

namespace QH {

/**
 * @brief The VersionIsReceived class
 */
class VersionIsReceived: public QH::PKG::AbstractData
{
    QH_PACKAGE(VersionIsReceived, "VersionIsReceived")

public:
    VersionIsReceived();

    // StreamBase interface
protected:
    QDataStream &fromStream(QDataStream &stream) override;
    QDataStream &toStream(QDataStream &stream) const override;
};

}
#endif // VERSIONISRECEIVED_H
