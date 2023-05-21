/*
 * Copyright (C) 2021-2023 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
*/


#ifndef BIGDATAPART_H
#define BIGDATAPART_H

#include "bigdatabase.h"


namespace QH {
namespace PKG {

/**
 * @brief The BigDataPart class This data class used for transporting big data files greater than 64kb.
 */
class HEARTSHARED_EXPORT BigDataPart : public BigDataBase
{
    QH_PACKAGE_AUTO("BigDataPart")

public:
    BigDataPart();

    /**
     * @brief getPakckageNumber This method return current package number
     * @return current pacakge number
     */
    int getPakckageNumber() const;

    /**
     * @brief setPakckageNumber This method sets new number of packagePart.
     * @param newPakckageNumber This is part package number.
     */
    void setPakckageNumber(int newPakckageNumber);

    bool isValid() const override;
    QString toString() const override;

    /**
     * @brief data This method return raw data of this package part.
     * @return raw data of this package part.
     */
    const QByteArray &data() const;

    /**
     * @brief setData this method sets new value raw data for pacakge part.
     * @param newData this is a new value raw data for pacakge part.
     */
    void setData(const QByteArray &newData);

protected:
    QDataStream &fromStream(QDataStream &stream) override;
    QDataStream &toStream(QDataStream &stream) const override;

private:
    int _pakckageNumber = 0;
    QByteArray _data;
};
}
}
#endif // BIGDATAPART_H
