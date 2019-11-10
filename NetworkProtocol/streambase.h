#ifndef STREAMBASE_H
#define STREAMBASE_H

#include <QByteArray>
#include <QVariantMap>
#include "networkprotocol_global.h"

class QDataStream;
namespace NetworkProtocol {

class Package;

class NETWORKPROTOCOLSHARED_EXPORT StreamBase
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


};
}
#endif // STREAMBASE_H
