/*
 * Copyright (C) 2018-2023 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
*/

#ifndef STREAMBASE_H
#define STREAMBASE_H

#include <QByteArray>
#include <QDataStream>
#include <QVariantMap>
#include "heart_global.h"
#include <type_traits>

class QDataStream;
namespace QH {

class Package;

/**
 * @brief The StreamBase class add support streaming data for all children classes.
 *  For correctly working all serializations functions you need to override fromStream and toStream methods.
 * All implementations of overridden method should be contains a invoke of method of base class.
 */
class HEARTSHARED_EXPORT StreamBase
{
public:
    StreamBase();
    virtual ~StreamBase();

    /**
     * @brief fromBytes This method provide initialization of object from byte array.
     * @return true if all good.
     */
    bool fromBytes(const QByteArray &data);

    /**
     * @brief toBytes This method convert a current object to bytes array.
     * @return bytes array for package.
     */
    QByteArray toBytes() const;

    /**
     * @brief fromBase64 This method provide initialization of object from the base64 string.
     * @param data This is input base64 data.
     * @note converting from the QString is slowly instand of QByteArray, so use the StreamBase::fromBase64(const QByteArray &data) method.
     * @return true if all good else false.
     */
    bool fromBase64(const QString &data);

    /**
     * @brief fromBase64 This method provide initialization of object from the base64 string.
     * @param data This is input base64 data.
     * @return true if all good else false.
     */
    bool fromBase64(const QByteArray &data);

    /**
     * @brief toBase64 This method convert a current object to the base64 string.
     * @return base64 string of this object.
     */
    QByteArray toBase64() const;

    /**
     * @brief This is wrapper over toStream.
     * @param stream This is qt data stream object.
     * @param obj This is serialized object.
     * @return stream object.
     */
    HEARTSHARED_EXPORT friend QDataStream& operator<< (QDataStream& stream, const StreamBase& obj);

    /**
     * @brief This is wraper over fromStream. For see more information see toStream method.
     * @param stream This is qt data stream object.
     * @param obj This is serialized object.
     * @return stream object.
     */
    HEARTSHARED_EXPORT friend QDataStream& operator>> (QDataStream& stream, StreamBase& obj);

    /**
     * @brief copy This is base copy method for all StreamBase structures.
     *  Default implementation it is copy from byteArray.
     * @param right input data object.
     * @return return value link to object.
     * @warning This method invoke the toBytes method 2 time and compare bigData arrays. So works is very slowly.
     */
    template<class T>
    T& copy(const StreamBase& right) {
        static_assert(std::is_base_of_v<StreamBase, T>,
                "The argument of the copy method must be base type of the StreamBase class");

        fromBytes(right.toBytes());
        return static_cast<T&>(*this);
    }

    template<class T>
    /**
     * @brief compare This method compare array signatures of this and @a right objects.
     * @param right This is comparable object.
     * @return true if right object is some of this.
     * @warning This method invoke the toBytes method 2 time and compare bigData arrays. So works is very slowly.
     */
    bool compare(const T& right) {
        static_assert(std::is_base_of_v<StreamBase, T>,
                "The argument of the compare method must be base type of the StreamBase class");

        if (static_cast<unsigned int>(typeid (T).hash_code()) != typeId()) {
            return false;
        }

        return toBytes() == right.toBytes();
    }

protected:

    /**
     * @brief parsingVersion this method return parsing version of Qt. By Default is 0 (last available parsing).
     * see https://doc.qt.io/qt-6/qdatastream.html#Version-enum
     * @return version number.
     */
    virtual int parsingVersion() const;

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

    /**
     * @brief typeId This method return id of type.
     * @return integer hash of type.
     */
    virtual unsigned int typeId() const;

};
}
#endif // STREAMBASE_H
