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
 * @brief The DbId class
 */
class NETWORKPROTOCOLSHARED_EXPORT DbId: public StreamBase
{
public:
    DbId();
    DbId(const QByteArray& raw);
    DbId(const QString& base64);

    bool fromBase64(const QByteArray& base64);
    bool fromRaw(const QByteArray& raw);

    QByteArray toBase64() const;
    QByteArray toRaw() const;

    bool isValid() const;

    QDataStream &fromStream(QDataStream &stream) override;
    QDataStream &toStream(QDataStream &stream) const override;

    friend bool operator== (const DbId& left, const DbId& other);

private:
    QByteArray _data;
};
}
#endif // DBID_H
