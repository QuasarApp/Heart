/*
 * Copyright (C) 2018-2020 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
*/

#ifndef STREAMBASE_H
#define STREAMBASE_H

#include <QByteArray>
#include <QVariantMap>
#include "networkprotocol_global.h"

class QDataStream;
namespace NP {

class Package;

/**
 * @brief The StreamBase class - this class add support for streaming data for all cheldren classes.
 */
class NETWORKPROTOCOLSHARED_EXPORT StreamBase
{
public:
    StreamBase();
    virtual ~StreamBase();

    /**
     * @brief fromBytes - private initialisation of object from byte array
     * @return true if all good
     */
    bool fromBytes(const QByteArray &data);

    /**
     * @brief toBytes
     * @return byte array for package
     */
    QByteArray toBytes() const;


    /**
     * @brief operator << it is wraper over toStream
     * @param stream
     * @param obj
     * @return stream
     */
    friend QDataStream& operator<< (QDataStream& stream, const StreamBase& obj);

    /**
     * @brief operator >> it is wraper over fromStream
     * @param stream
     * @param obj
     * @return
     */
    friend QDataStream& operator>> (QDataStream& stream, StreamBase& obj);


protected:
    /**
     * @brief fromStream
     * @param stream
     * @return stream
     */
    virtual QDataStream& fromStream(QDataStream& stream) = 0;

    /**
     * @brief toStream
     * @param stream
     * @return stream
     */
    virtual QDataStream& toStream(QDataStream& stream) const = 0;

};
}
#endif // STREAMBASE_H
