#ifndef CRYPTOPAIRKEYS_H
#define CRYPTOPAIRKEYS_H

#include "streambase.h"


namespace QH {

/**
 * @brief The CryptoPairKeys class contains pair keys. This is wraper for cripto grafick keys.
 */
class HEARTSHARED_EXPORT CryptoPairKeys: public StreamBase
{
public:
    CryptoPairKeys();
    CryptoPairKeys(const QByteArray& pubKey, const QByteArray& privKey);

    /**
     * @brief isValid - This Method check a internal keys.
     * @return true if this objcet contains a valid pair keys
     */
    bool isValid() const;

    /**
     * @brief privKey This method retun private key of current keys pair.
     * @return return byteArray of private key.
     */
    QByteArray privKey() const;

    /**
     * @brief setPrivKey This method set a private key for keys pair.
     * @param privKey new byte array of private pair keys.
     */
    void setPrivKey(const QByteArray &privKey);

    /**
     * @brief publicKey This method return public key of array.
     * @return pubkic key of current key pair.
     */
    QByteArray publicKey() const;

    /**
     * @brief setPublicKey This method set new value of pablickKey
     * @param publicKey new value of publicKey
     */
    void setPublicKey(const QByteArray &publicKey);

    /**
     * @brief bits This method returm size pair of crypto keys in bits
     * @return bits size of keys pair.
     */
    int bits() const;

    /**
     * @brief setBits This method sets new value of keys pair
     * @param bits This is new value of size keys.
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

/**
     * @brief qHash This functions calc crc32 hash code of  CryptoPairKeys object
     * @param value -
     * @return crc32 hash code
     */
    uint qHash(const CryptoPairKeys& value);
}
#endif // CRYPTOPAIRKEYS_H
