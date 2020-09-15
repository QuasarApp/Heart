#ifndef CRYPTOPAIRKEYS_H
#define CRYPTOPAIRKEYS_H

#include "streambase.h"


namespace NP {

/**
 * @brief The CryptoPairKeys class contains pair keys.
 */
class NETWORKPROTOCOLSHARED_EXPORT CryptoPairKeys: public StreamBase
{
public:
    CryptoPairKeys();
    CryptoPairKeys(const QByteArray& pubKey, const QByteArray& privKey);

    /**
     * @brief isValid
     * @return true if this objcet contains pair keys
     */
    bool isValid() const;

    QByteArray privKey() const;
    void setPrivKey(const QByteArray &privKey);

    QByteArray publicKey() const;
    void setPublicKey(const QByteArray &publicKey);

    /**
     * @brief bits
     * @return bits size of keys pair
     */
    int bits() const;

    /**
     * @brief setBits
     * @param bits
     */
    void setBits(int bits);

    friend bool operator != (const CryptoPairKeys& left, const CryptoPairKeys& right);
    friend bool operator == (const CryptoPairKeys& left, const CryptoPairKeys& right);

    // StreamBase interface
protected:
    QDataStream &fromStream(QDataStream &stream);
    QDataStream &toStream(QDataStream &stream) const;

private:
    QByteArray _privKey;
    QByteArray _publicKey;

    int _bits;
};

    uint qHash(const CryptoPairKeys& value);
}
#endif // CRYPTOPAIRKEYS_H
