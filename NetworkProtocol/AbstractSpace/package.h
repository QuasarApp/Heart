/*
 * Copyright (C) 2018-2020 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
*/

#ifndef ABSTRACTPACKAGE_H
#define ABSTRACTPACKAGE_H
#include "header.h"
#include "networkprotocol_global.h"
#include <streambase.h>

#include <QByteArray>

namespace NP {

class Abstract;
/**
 * @brief The Package struct
 */
struct NETWORKPROTOCOLSHARED_EXPORT Package: public StreamBase {
    /**
     * @brief hdr - header of package
     */
    Header hdr;
    /**
     * @brief data - source data of package
     */
    QByteArray data;

    Package();

    /**
     * @brief isValid
     * @return true if package is valid
     */
    virtual bool isValid() const;

    /**
     * @brief reset - reset all data and set for package invalid status
     */
    virtual void reset();

    /**
     * @brief toString - convert pakcage information to string label
     * @return string
     */
    QString toString() const;

    virtual ~Package() = default;


    // StreamBase interface
protected:
    QDataStream &fromStream(QDataStream &stream) override;
    QDataStream &toStream(QDataStream &stream) const override;
};

}
#endif // ABSTRACTPACKAGE_H
