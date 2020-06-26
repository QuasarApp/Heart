/*
 * Copyright (C) 2018-2020 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
*/

#ifndef DBID_H
#define DBID_H

#include <QByteArray>
#include <streambase.h>

namespace NP {

/**
 * @brief The DbId class. General class for work with database id.
 * Database id is '1byte of prefix + hash (sha256)' from object value.
 */
class NETWORKPROTOCOLSHARED_EXPORT DbId: public StreamBase
{
public:
    DbId();
    DbId(const QByteArray& raw);
    DbId(const QString& base64);

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
    QByteArray toRaw() const;

    /**
     * @brief isValid
     * @return
     */
    bool isValid() const;

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

    friend bool operator== (const DbId& left, const DbId& other);

private:
    QByteArray _data;
};
}
#endif // DBID_H
