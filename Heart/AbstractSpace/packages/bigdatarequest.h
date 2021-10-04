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
public:
    BigDataRequest();

    /**
     * @brief neededParts This method retunr list of missng parts ids.
     * @return list of the missing parts ids.
     * @see BigDataRequest::isFinishedSuccessful
     */
    const QList<int> &neededParts() const;

    /**
     * @brief setNeededParts This method sets new list of missing package parts.
     * @param newNeededParts new list of the missing pacakge parts.
     */
    void setNeededParts(const QList<int> &newNeededParts);

    /**
     * @brief isFinishedSuccessful This method return true if the pacakge donwload sucessfull else false.
     * @return true if the pacakge donwload sucessfull else false.
     */
    bool isFinishedSuccessful() const;


    // StreamBase interface
protected:
    QDataStream &fromStream(QDataStream &stream) override;
    QDataStream &toStream(QDataStream &stream) const override;

private:
    QList<int> _neededParts;

};

}
}

#endif // BIGDATAREQUEST_H
