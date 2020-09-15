#ifndef ICRYPTO_H
#define ICRYPTO_H

#include <QList>
#include <QThread>
#include "heart_global.h"
#include <QByteArray>
#include <QHash>
#include <QSet>
#define RAND_KEY ""

class QMutex;

namespace QH {

class CryptoPairKeys;

/**
 * @brief The ICrypto class provide cryptografu functionality.
 *  this is interface for decaration of KeyStorage classes.
 */
class HEARTSHARED_EXPORT ICrypto
{

public:
    ICrypto();
    virtual ~ICrypto();

    /**
     * @brief isValid
     * @return true if the crypto object is valid.
     */
    virtual bool isValid() const = 0;


    /**
     * @brief crypt
     * @param data - pointer to data array for crypting.
     * @note data as ben changed after call this method.
     * @param publicKey - key for crypting data
     * @return true if function finished seccussful
     */
    virtual bool crypt(QByteArray *data, const QByteArray& publicKey) = 0;

    /**
     * @brief decrypt
     * @param cryptedData - pointer to data array for decrypting.
     * @note cryptedData as ben changed after call this method.
     * @param privateKey
     * @return true if function finished seccussful
     */
    virtual bool decrypt(QByteArray *cryptedData, const QByteArray& privateKey) = 0;

    /**
     * @brief sign
     * @param data - pointer to data array for signed.
     * @note data as ben changed after call this method.
     * @param privateKey
     * @return true if function finished seccussful
     */
    virtual bool sign(QByteArray* data, const QByteArray& privateKey) = 0;

    /**
     * @brief extractSign - extract sign from signed byteArray
     * @param data - signed message.
     * @return return array of sign.
     */
    virtual QByteArray extractSign(const QByteArray& data) = 0;

    /**
     * @brief concatSign
     * @param data - message data
     * @return signed message
     */
    virtual QByteArray concatSign(const QByteArray& data, const QByteArray& sign) = 0;

    /**
     * @brief check
     * @param publicKey
     * @return true if function finished seccussful and signedData is valid.
     */
    virtual bool check(const QByteArray& signedData, const QByteArray& publicKey) = 0;

    /**
     * @brief generate a new key. Default implementation do nothing.
     * @note Override this method for create of new class with new keys type.
     * @param genesis
     * @return crypto pair keys
     */
    virtual CryptoPairKeys generate(const QByteArray& genesis = {}) const = 0;

};

}
#endif // CRYPTO_H
