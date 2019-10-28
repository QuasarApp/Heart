#ifndef STREAMBASE_H
#define STREAMBASE_H

#include <QByteArray>
#include <QVariantMap>
#include "clientprotocol_global.h"

class QDataStream;
namespace ClientProtocol {

class Package;

class CLIENTPROTOCOLSHARED_EXPORT StreamBase
{
public:
    StreamBase();
    virtual ~StreamBase();

    /**
     * @brief fromBytes - private initialisation of object from byte array
     * @return true if all good
     */
    bool fromBytes(const QByteArray&data);

    /**
     * @brief toBytes
     * @return byte array for package
     */
    QByteArray toBytes() const;

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

    /**
     * @brief fromVariantMap
     * @param map
     * @return
     */
    virtual QVariantMap& fromVariantMap(QVariantMap& map) = 0;

    /**
     * @brief toVariantmap
     * @param map
     * @return
     */
    virtual QVariantMap& toVariantmap(QVariantMap& map) const = 0;

};
}
#endif // STREAMBASE_H
