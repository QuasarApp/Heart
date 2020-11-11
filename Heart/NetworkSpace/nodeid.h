/*
 * Copyright (C) 2018-2020 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
*/

#ifndef BaseId_H
#define BaseId_H

#include <QByteArray>
#include <streambase.h>

namespace QH {

/**
 * @brief The NodeId class. General class for work with database id.
 * The Database id is hash (sha256) from object value.
 *  For more information about get hash code of databae objects see the DBObject::generateId method.
 */
class HEARTSHARED_EXPORT NodeId: public StreamBase
{
public:
    NodeId();

    /**
     * @brief NodeId This contructor init the id from int value.
     * @param val This is new data value.
     */
    NodeId(unsigned int val);

    /**
     * @brief NodeId This contructor init this object from raw bytearray.
     * @param raw
     */
    NodeId(const QByteArray& raw);

    /**
     * @brief NodeId This constructor init this object from base 64 codec.
     * @param base64 string with base 64 codec. This string will bew converted to a bytes array.
     */
    NodeId(const QString& base64);

    /**
     * @brief fromBase64 This method is wraper of QByteArray::fromBase64 method of Qt.
     * @param base64 string with base 64 codec. This string will bew converted to a bytes array.
     * @return True if the initialize of this object fuinished successful.
     */
    bool fromBase64(const QByteArray& base64);

    /**
     * @brief fromRaw This method conver a raw QByteArray to the BaseId key.
     * @param raw This is raw bytes array.
     * @return True if the initialize of this object fuinished successful.
     */
    bool fromRaw(const QByteArray& raw);

    /**
     * @brief fromRaw This is wraper initializing from the C bytes array.
     * @param data  This is raw data array.
     * @param len This is length of data.
     * @return True if the initialize of this object fuinished successful.
     */
    bool fromRaw(const char* data, int len);

    /**
     * @brief toBase64 This method convert this object to the base 64 string.
     * @return The base64 string value.
     */
    QByteArray toBase64() const;

    /**
     * @brief toRaw This method convert this object to raw bytes array.
     * @return raw data array.
     */
    const QByteArray& toRaw() const;

    /**
     * @brief isValid This method check this object to valid.
     * @return true if object is valid.
     */
    virtual bool isValid() const;

    /**
     * @brief clear This method reset all data members of this object.
     */
    void clear();

    QDataStream &fromStream(QDataStream &stream) override;
    QDataStream &toStream(QDataStream &stream) const override;

    friend bool operator== (const NodeId& left, const NodeId& other);

private:
    QByteArray _data;
};


/**
 * @brief qHash This function calc the 32bit hash of the BaseId object.
 * @param object This is input object data.
 * @return unsigned int 32 value.
 */
uint qHash(const QH::NodeId& object);

}


#endif // BaseId_H
