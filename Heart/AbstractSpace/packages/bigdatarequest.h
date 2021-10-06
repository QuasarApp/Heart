/*
 * Copyright (C) 2021-2021 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
*/


#ifndef BIGDATAREQUEST_H
#define BIGDATAREQUEST_H
#include "bigdatabase.h"

namespace QH {
namespace PKG {

/**
 * @brief The BigDataRequest class contains missing package parts for redownload. If missing parts count is 0 then pacakge donwloaded successful.
 * @see BigDataRequest::isFinishedSuccessful
 */
class HEARTSHARED_EXPORT BigDataRequest: public BigDataBase
{
    QH_PACKAGE(BigDataRequest, "BigDataRequest")

public:
    BigDataRequest();

    // StreamBase interface

    /**
     * @brief currentPart This method return current part number.
     * @return current part numbep of moved pacakged.
     */
    int currentPart() const;

    /**
     * @brief setCurrentPart This method sets current new value of current page
     * @param newCurrentPart This is new value of current part.
     */
    void setCurrentPart(int newCurrentPart);

protected:
    QDataStream &fromStream(QDataStream &stream) override;
    QDataStream &toStream(QDataStream &stream) const override;

private:
    int _currentPart = 0;


};

}
}

#endif // BIGDATAREQUEST_H
