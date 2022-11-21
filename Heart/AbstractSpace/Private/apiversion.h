//#
//# Copyright (C) 2020-2022 QuasarApp.
//# Distributed under the GPLv3 software license, see the accompanying
//# Everyone is permitted to copy and distribute verbatim copies
//# of this license document, but changing it is not allowed.
//#

#ifndef APPVERSION_H
#define APPVERSION_H

#include <abstractdata.h>


namespace RC {

/**
 * @brief The APIVersion class This is base pacakge class that send information about supported api version on the paired node
 */
class APIVersion: public QH::PKG::AbstractData
{

public:
    APIVersion();

    /**
     * @brief minimum This is minimum supported api version
     * @return minimum supported api version
     */
    unsigned short minimum() const;

    /**
     * @brief setMinimum This mehod sets new value for the minimum supported api version.
     * @param newMinimum This is new value of the minimum supported api version.
     */
    void setMinimum(unsigned short newMinimum);

    /**
     * @brief maximum This is maximum supported api version
     * @return maximum supported api version
     */
    unsigned short maximum() const;

    /**
     * @brief setMaximum This mehod sets new value for the maximum supported api version.
     * @param newMaximum This is new value of the maximum supported api version.
     */
    void setMaximum(unsigned short newMaximum);

    /**
     * @brief APINameKey This is api
     * @return
     */
    unsigned int APINameKey() const;
    void setAPINameKey(unsigned int newAPINameKey);

    static unsigned short command(){return PROTOCKOL_VERSION_COMMAND;}
    static QString commandText(){return "PROTOCKOL_VERSION_COMMAND";}
    unsigned short cmd() const override {return APIVersion::command();}
    QString cmdString() const override {return APIVersion::commandText();}

protected:
    unsigned int localCode() const override {return typeid(APIVersion).hash_code();}
    QDataStream &fromStream(QDataStream &stream) override;
    QDataStream &toStream(QDataStream &stream) const override;

private:
    unsigned int _APINameKey = 0;
    unsigned short _minimum = 0;
    unsigned short _maximum = 0;

};

}

#endif // APPVERSION_H
