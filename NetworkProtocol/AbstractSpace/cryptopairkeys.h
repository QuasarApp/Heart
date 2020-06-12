#ifndef CRYPTOPAIRKEYS_H
#define CRYPTOPAIRKEYS_H

#include <QByteArray>
#include "networkprotocol_global.h"

namespace NP {

/**
 * @brief The CryptoPairKeys class contains pair keys.
 */
class NETWORKPROTOCOLSHARED_EXPORT CryptoPairKeys
{
public:
    CryptoPairKeys();
    /**
     * @brief isValid
     * @return true if this objcet contains pair keys
     */
    bool isValid();

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

private:
    QByteArray _privKey;
    QByteArray _publicKey;

    int _bits;

};
}
#endif // CRYPTOPAIRKEYS_H
