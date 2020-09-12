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

namespace NP {

/**
 * @brief The BaseId class. General class for work with database id.
 * Database id is '1byte of prefix + hash (sha256)' from object value.
 */
class NETWORKPROTOCOLSHARED_EXPORT BaseId: public StreamBase
{
public:
    BaseId();
    BaseId(unsigned int val);

    BaseId(const QByteArray& raw);
    BaseId(const QString& base64);

    /**
     * @brief fromBase64
     * @param base64
     * @return
     */
    bool fromBase64(const QByteArray& base64);

    /**
     * @brief fromRaw
     * @param raw
     * @return
     */
    bool fromRaw(const QByteArray& raw);

    /**
     * @brief toBase64
     * @return
     */
    QByteArray toBase64() const;

    /**
     * @brief toRaw
     * @return
     */
    const QByteArray& toRaw() const;

    /**
     * @brief isValid
     * @return true if object is valid
     */
    virtual bool isValid() const;

    /**
     * @brief clear
     * @return
     */
    void clear();

    /**
     * @brief prefix - return prefix of id. if id is not valid return 0.
     * @return prefix of id
     */
    unsigned char prefix() const;

    QDataStream &fromStream(QDataStream &stream) override;
    QDataStream &toStream(QDataStream &stream) const override;

    friend bool operator== (const BaseId& left, const BaseId& other);

private:
    QByteArray _data;
};


uint qHash(const NP::BaseId& object);

}


#endif // BaseId_H
