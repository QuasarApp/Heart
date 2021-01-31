/*
 * Copyright (C) 2018-2021 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
*/

#ifndef STREAMBASE_H
#define STREAMBASE_H

#include <QByteArray>
#include <QVariantMap>
#include "heart_global.h"

class QDataStream;
namespace QH {

class Package;

/**
 * @brief The StreamBase class add support streaming data for all children classes.
 *  For correctly working all serializations functions you need to override fromStream and toStream methods.
 * All implementations of overriden method should be contains a invoke of method of base class.
 */
class HEARTSHARED_EXPORT StreamBase
{
public:
    StreamBase();
    virtual ~StreamBase();

    /**
     * @brief fromBytes This method method provide initialisation of object from byte array.
     * @return true if all good
     */
    bool fromBytes(const QByteArray &data);

    /**
     * @brief toBytes This method convert a current object to bytes array.
     * @return bytes array for package
     */
    QByteArray toBytes() const;


    /**
     * @brief This is wraper over toStream
     * @param stream This is qt data stream object.
     * @param obj This is serialized object.
     * @return stream object
     */
    friend QDataStream& operator<< (QDataStream& stream, const StreamBase& obj);

    /**
     * @brief This is wraper over toStream
     * @param stream This is qt data stream object.
     * @param obj This is serialized object.
     * @return stream object
     */
    friend QDataStream& operator>> (QDataStream& stream, StreamBase& obj);

    /**
     * @brief operator = This is base copy operator for all StreamBase structures.
     *  Default implementation it is copy from byteArray.
     * @param righ input data object.
     * @return return lvalue link to object.
     */
    StreamBase& operator=(const StreamBase &righ);

protected:

    /**
     * @brief fromStream This method should be read all bytes from the stream object and full the current object.
     * @note The implementation of this method should be invoke a method of base class.
     * @param stream This is Qt stream object.
     * @return Qt stream object.
     *
     * Examle of base simple implementation:
     *
     *  \code{cpp}
        QDataStream &ExampleClass::fromStream(QDataStream &stream) const {
            StreamBase::toStream(stream);
            stream >> exampleMember;
            return stream;
        }
     *  \endcode
     */
    virtual QDataStream& fromStream(QDataStream& stream) = 0;

    /**
     * @brief fromStream This method should be write all members of the current object to the stream object.
     * @note The implementation of this method should be invoke a method of base class.
     * @param stream This is Qt stream object.
     * @return Qt stream object.
     *
     * Examle of base simple implementation:
     * \code{cpp}
        QDataStream &ExampleClass::toStream(QDataStream &stream) const {
            StreamBase::toStream(stream);
            stream << exampleMember;
            return stream;
        }
     * \endcode
     */
    virtual QDataStream& toStream(QDataStream& stream) const = 0;

};
}
#endif // STREAMBASE_H
