/*
 * Copyright (C) 2021-2025 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
*/


#ifndef BIGDATABASE_H
#define BIGDATABASE_H

#include "abstractdata.h"

namespace QH {
namespace PKG {

/**
 * @brief The BigDataBase class This is base class of big data parts.  This class contains id of movving pacakge.
 */
class HEARTSHARED_EXPORT BigDataBase: public AbstractData
{
    QH_PACKAGE("BigDataBase")

public:
    BigDataBase();

    /**
     * @brief getPackageId This method return package id that this part moving.
     * @return id of package that this part moving.
     */
    int packageId() const;

    /**
     * @brief setPackageId This method sets new id of package that this package moving
     * @param newPackageId This is new value of the id of pacakge.
     */
    void setPackageId(int newPackageId);

    bool isValid() const override;
    QString toString() const override;

    // StreamBase interface
protected:
    QDataStream &fromStream(QDataStream &stream) override;
    QDataStream &toStream(QDataStream &stream) const override;

private:
    int _packageId = 0;

};

}
}
#endif // BIGDATABASE_H
