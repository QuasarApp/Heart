/*
 * Copyright (C) 2018-2020 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
*/

#ifndef ACCSESTOKEN_H
#define ACCSESTOKEN_H

#include "streambase.h"

#include <QByteArray>
#include <QDateTime>
#include <networkprotocol_global.h>

namespace NP {

/**
 * @brief The AccessToken class - contains info about token and duration of token
 */
class NETWORKPROTOCOLSHARED_EXPORT AccessToken : public StreamBase
{
public:

    enum Duration: int {
        Secund = 1,
        Minute = 60 * Secund,
        Hour = 60 * Minute,
        Day = 24 * Hour,
        Week = 7 * Day,
        Month = 30 * Day,
        Year = 365 * Day,
    };

    /**
     * @brief AccsesToken - this constructor create token on duration (s)
     * @param duration - value as secundes
     * @param entropy - random bytes array for generate token
     */
    explicit AccessToken(int duration, const QByteArray& entropy = "");

    /**
     * @brief AccsesToken - constructor by default create not valid token
     */
    explicit AccessToken();

    /**
     * @brief AccessToken - copy constructor
     * @param other
     */
    AccessToken(const AccessToken& other);

    /**
     * @brief isValid
     * @return return true if token is valid;
     */
    bool isValid() const;

    /**
     * @brief clear - reset all data of token
     */
    void clear();

    friend QDataStream& operator<<(QDataStream& stream, const AccessToken& token);
    friend QDataStream& operator>>(QDataStream& stream, AccessToken& token);
    bool operator == (const AccessToken& other) const;
    bool operator != (const AccessToken& other) const;

    AccessToken &operator =(const AccessToken& other);

    friend uint qHash(const AccessToken& token);

private:
    QByteArray _data;
    int _duration;

    // StreamBase interface
    QDataStream &fromStream(QDataStream &stream);
    QDataStream &toStream(QDataStream &stream) const;

    QByteArray generate(const QByteArray &user);
};

}

#endif // ACCSESTOKEN_H
