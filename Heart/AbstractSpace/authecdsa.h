//#
//# Copyright (C) 2021-2022 QuasarApp.
//# Distributed under the GPLv3 software license, see the accompanying
//# Everyone is permitted to copy and distribute verbatim copies
//# of this license document, but changing it is not allowed.
//#


#ifndef AUTHECDSA_H
#define AUTHECDSA_H

#include "abstractdata.h"
#include <asynckeysauth.h>

namespace QH {

/**
 * @brief The AuthECDSA class is ecdsa implementation of the Async authentication. This implementation based on Openssl library.
 */
class AuthECDSA: public QH::AsyncKeysAuth
{

public:
    AuthECDSA();

    /**
     * @brief makeKeys This static method generate the public and private keys of the ECDSA.
     * @param pubKey This is result public key.
     * @param privKey This is result private key.
     * @return true if keys generated successful.
     */
    static bool makeKeys(QByteArray &pubKey, QByteArray &privKey);
    
    // AsyncKeysAuth interface
protected:
    QByteArray signMessage(const QByteArray &inputData, const QByteArray &key) const override;
    bool checkSign(const QByteArray &inputData, const QByteArray &signature, const QByteArray &key) const override;

};


}


#endif // AUTHECDSA_H
