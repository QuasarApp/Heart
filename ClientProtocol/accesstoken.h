#ifndef ACCSESTOKEN_H
#define ACCSESTOKEN_H

#include "streambase.h"

#include <QByteArray>
#include <QDateTime>
#include <clientprotocol_global.h>

namespace ClientProtocol {

/**
 * @brief The AccessToken class - contains info about token and duration of token
 */
class CLIENTPROTOCOLSHARED_EXPORT AccessToken : public StreamBase
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

    friend QDataStream& operator<<(QDataStream& stream, const AccessToken& token);
    friend QDataStream& operator>>(QDataStream& stream, AccessToken& token);
    bool operator == (const AccessToken& other) const;
    AccessToken &operator =(const AccessToken& other);

    friend unsigned int qHash(const AccessToken& token);

private:
    QByteArray _data;
    int _duration;

    // StreamBase interface
    QDataStream &fromStream(QDataStream &stream);
    QDataStream &toStream(QDataStream &stream) const;

    QByteArray generate(const QByteArray &user);
};

uint qHash(const AccessToken& token);

}

#endif // ACCSESTOKEN_H
