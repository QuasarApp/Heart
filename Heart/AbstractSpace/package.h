/*
 * Copyright (C) 2018-2021 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
*/

#ifndef ABSTRACTPACKAGE_H
#define ABSTRACTPACKAGE_H
#include "header.h"
#include "heart_global.h"
#include "streambase.h"

#include <QByteArray>

namespace QH {

class Abstract;
/**
 * @brief The Package struct. This is base structure for transporting data by network between QH nodes.
 * The Package contains a 12 bytes header and Package::data array. The size on the header should be equals size of Package::data array.
 *
 */
class HEARTSHARED_EXPORT Package: public StreamBase {

public:
    /**
     * @brief hdr This is header of package. For more information see the Header struct.
     */
    Header hdr;
    /**
     * @brief data This is source data of package.
     */
    QByteArray data;

    Package();
    virtual ~Package() = default;

    /**
     * @brief isValid This method validation a current package.
     * Default implementation is checked a header and compare a size of package in header and size of source data.
     * @return true if package is valid.
     */
    virtual bool isValid() const;

    /**
     * @brief reset This method reset all data and set for package invalid status.
     */
    virtual void reset();

    /**
     * @brief toString This method convert a package information to a string label.
     * @return string value of the package.
     */
    QString toString() const;

    /**
     * @brief calcHash This method recalc hash sum for this pacakge.
     * @return int32 hash of pacakge.
     */
    unsigned int calcHash() const;

    /**
     * @brief maximumSize This method return maximu size of pacakge. If pacakge large the maximum size then package will separate to BigDataPart in sending.
     * @return size in bytes of pacakge.
     */
    static int maximumSize();

    // StreamBase interface
protected:
    QDataStream &fromStream(QDataStream &stream) override;
    QDataStream &toStream(QDataStream &stream) const override;
};

}
#endif // ABSTRACTPACKAGE_H
