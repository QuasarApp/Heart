/*
 * Copyright (C) 2018-2025 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
*/

#ifndef ACCSESTOKEN_H
#define ACCSESTOKEN_H

#include "streambase.h"

#include <QByteArray>
#include <QDateTime>
#include <heart_global.h>

namespace QH {

/**
 * @brief The AccessToken class contains information duration of the access.
 * Token are byte array for validation access users or another network member on nodes.
 *  In usually cases toke add permissions for database objects.
 */
class HEARTSHARED_EXPORT AccessToken : public StreamBase
{
public:

    /**
     * @brief The Duration enum. This enum contains a long time unit.
     */
    enum Duration: int {
        /// The one second
        Second = 1,
        /// The one minute
        Minute = 60 * Second,
        /// The one hour
        Hour = 60 * Minute,
        /// The one day
        Day = 24 * Hour,
        /// The one Week
        Week = 7 * Day,
        /// The one Month
        Month = 30 * Day,
        /// The one year
        Year = 365 * Day,
    };

    /**
     * @brief AccessToken This constructor create token on duration (in seconds).
     * @param duration This is value in seconds.
     * @param entropy This is random bytes array for generate token. By Default is empty string.
     */
    explicit AccessToken(int duration, const QByteArray& entropy = "");

    /**
     * @brief AccessToken - Create token from bytes array of another token.
     * @param other This is bytes array of another token value.
     */
    explicit AccessToken(const QByteArray& other);

    /**
     * @brief AccessToken constructor by default create not valid token.
     */
    explicit AccessToken();

    /**
     * @brief AccessToken copy constructor.
     * @param other
     */
    AccessToken(const AccessToken& other);

    /**
     * @brief isValid This method check this toke to valid.
     * @return Return true if token is valid.
     */
    bool isValid() const;

    /**
     * @brief clearThis Method reset all data of token.
     */
    void clear();

    /**
     * @brief operator == compare left and right values with type AccessToken.
     * @param other This is another object of the AccessTokens.
     * @return True if 2 object is equals.
     */
    bool operator == (const AccessToken& other) const;

    /**
     * @brief operator != compare left and right values with type AccessToken.
     * @param other This is another object of the AccessTokens.
     * @return False if 2 object is equals.
     */
    bool operator != (const AccessToken& other) const;

    /**
     * @brief operator = this is operator of copy objects.
     * @param other
     * @return
     */
    AccessToken &operator =(const AccessToken& other);

protected:
    QDataStream &fromStream(QDataStream &stream);
    QDataStream &toStream(QDataStream &stream) const;

private:
    QByteArray _data;
    int _duration;

    // StreamBase interface


    QByteArray generate(const QByteArray &user);
};

}

#endif // ACCSESTOKEN_H
