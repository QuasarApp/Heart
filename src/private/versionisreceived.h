//#
//# Copyright (C) 2020-2023 QuasarApp.
//# Distributed under the lgplv3 software license, see the accompanying
//# Everyone is permitted to copy and distribute verbatim copies
//# of this license document, but changing it is not allowed.
//#


#ifndef VERSIONISRECEIVED_H
#define VERSIONISRECEIVED_H

#include <abstractdata.h>

namespace QH {
namespace PKG {

/**
 * @brief The VersionIsReceived class base class for confirmed versions.
 */
class VersionIsReceived: public QH::PKG::AbstractData
{

public:
    VersionIsReceived();
    static unsigned short version(){return 0;}

    static unsigned short command(){return PROTOCKOL_VERSION_RECEIVED_COMMAND;}
    static QString commandText(){return "PROTOCKOL_VERSION_RECEIVED_COMMAND";}
    unsigned short cmd() const override {return VersionIsReceived::command();}
    QString cmdString() const override {return VersionIsReceived::commandText();}
    unsigned short ver() const override {return VersionIsReceived::version();}


    // StreamBase interface
protected:
    QDataStream &fromStream(QDataStream &stream) override;
    QDataStream &toStream(QDataStream &stream) const override;
};
}
}
#endif // VERSIONISRECEIVED_H
