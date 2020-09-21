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
 * @brief The ICrypto class provide cryptographic functionality.
 *  this is interface for decaration of KeyStorage classes.
 */
class HEARTSHARED_EXPORT ICrypto
{

public:
    ICrypto();
    virtual ~ICrypto();

    /**
     * @brief isValid  check crypto object is valid
     * @return true if the crypto object is valid.
     */
    virtual bool isValid() const = 0;


    /**
     * @brief crypt This method crypt input data using public key.
     * @param data This is pointer to data array for crypting.
     * @note data as ben changed after call this method.
     * @param publicKey This is key for crypting data
     * @return true if function finished seccussful
     */
    virtual bool crypt(QByteArray *data, const QByteArray& publicKey) = 0;

    /**
     * @brief decrypt This method
     * @param cryptedData This is pointer to data array for decrypting.
     * @note cryptedData has been changed after call this method.
     * @param privateKey This is key for decripting data message
     * @return true if function finished seccussful
     */
    virtual bool decrypt(QByteArray *cryptedData, const QByteArray& privateKey) = 0;

    /**
     * @brief sign  This methdo sign a input message using privateKey
     * @param data This is pointer to data array for signed.
     * @note data has been changed after call this method.
     * @param privateKey This is key for sign a message.
     * @return true if function finished seccussful
     */
    virtual bool sign(QByteArray* data, const QByteArray& privateKey) = 0;

    /**
     * @brief extractSign This method extract sign from signed byteArray
     * @param data - signed message.
     * @return return array of sign.
     */
    virtual QByteArray extractSign(const QByteArray& data) = 0;

    /**
     * @brief concatSign This method make a concatenation between a message data and a sign of message.
     * @param data This is message data
     * @param sign This is signa of massage
     * @return signed message
     */
    virtual QByteArray concatSign(const QByteArray& data, const QByteArray& sign) = 0;

    /**
     * @brief check This method validation signed message
     * @param signedData This is array of signed data.
     * @param publicKey This is public key for validation of message.
     * @return true if function finished seccussful and signedData is valid.
     */
    virtual bool check(const QByteArray& signedData, const QByteArray& publicKey) = 0;

    /**
     * @brief generate This method is generate a new key pair. Default implementation do nothing.
     * @note Override this method for create of new class with new keys type.
     * @param genesis This is genesis bytes Array. if is posable add support generate keys pair from genessis data.
     * @return crypto pair keys
     */
    virtual CryptoPairKeys generate(const QByteArray& genesis = {}) const = 0;

};

}
#endif // CRYPTO_H
