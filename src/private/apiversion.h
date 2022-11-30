//#
//# Copyright (C) 2020-2022 QuasarApp.
//# Distributed under the GPLv3 software license, see the accompanying
//# Everyone is permitted to copy and distribute verbatim copies
//# of this license document, but changing it is not allowed.
//#

#ifndef APPVERSION_H
#define APPVERSION_H

#include "distversion.h"
#include <abstractdata.h>


namespace QH {

namespace PKG {


/**
 * @brief The APIVersion class This is base pacakge class that send information about supported api version on the paired node
 */
class APIVersion: public QH::PKG::AbstractData
{

public:
    APIVersion();

    static unsigned short command(){return PROTOCKOL_VERSION_COMMAND;}
    static QString commandText(){return "PROTOCKOL_VERSION_COMMAND";}
    unsigned short cmd() const override {return APIVersion::command();}

    unsigned short cmdOld() const override {return APIVersion::command();}

    QString cmdString() const override {return APIVersion::commandText();}

    const VersionData &version() const;
    void setVersion(const VersionData &newVersion);

    bool isValid() const override;

protected:
    unsigned int localCode() const override {return typeid(APIVersion).hash_code();}
    QDataStream &fromStream(QDataStream &stream) override;
    QDataStream &toStream(QDataStream &stream) const override;

private:
    VersionData _version;

};

}
}
#endif // APPVERSION_H
