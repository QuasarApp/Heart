//#
//# Copyright (C) 2021-2023 QuasarApp.
//# Distributed under the GPLv3 software license, see the accompanying
//# Everyone is permitted to copy and distribute verbatim copies
//# of this license document, but changing it is not allowed.
//#


#ifndef QH_ECDSA_SSL_1_1_H
#define QH_ECDSA_SSL_1_1_H
#include "heart_global.h"

#ifdef USE_HEART_SSL

#include "icrypto.h"

#include <openssl/ec.h>      // for EC_GROUP_new_by_curve_name, EC_GROUP_free, EC_KEY_new, EC_KEY_set_group, EC_KEY_generate_key,EC_KEY_free

namespace QH {

/**
 * @brief The ECDSASSL11 class is ecdsa implementation of the Async authentication. This implementation based on Openssl library.
 * @note This class compatibility only with ssl 1.1 and ssl 3.0 (depricated fundtions).
 */
class HEARTSHARED_EXPORT ECDSASSL11: public QH::ICrypto
{

public:
    ECDSASSL11();

    /**
     * @brief makeKeys This static method generate the public and private keys of the ECDSA.
     * @param pubKey This is result public key.
     * @param privKey This is result private key.
     * @return true if keys generated successful.
     */
    static bool makeKeys(QByteArray &pubKey, QByteArray &privKey);
    
protected:
    QByteArray signMessage(const QByteArray &inputData, const QByteArray &key) const override;
    bool checkSign(const QByteArray &inputData, const QByteArray &signature, const QByteArray &key) const override;

    /**
     * @brief decript This method has empty implementation.
     * @return empty array.
     */
    QByteArray decript(const QByteArray &message, const QByteArray &key) override;

    /**
     * @brief encript This method has empty implementation.
     * @return empty array.
     */
    QByteArray encript(const QByteArray &message, const QByteArray &key) override;

private:
    static bool prepareKeyAdnGroupObjects(EC_KEY **eckey, EC_GROUP **ecgroup);

   };

}

#endif

#endif // QH_ECDSA_SSL_1_1_H
