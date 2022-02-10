//#
//# Copyright (C) 2021-2022 QuasarApp.
//# Distributed under the GPLv3 software license, see the accompanying
//# Everyone is permitted to copy and distribute verbatim copies
//# of this license document, but changing it is not allowed.
//#


#include "authecdsa.h"

#include <openssl/ec.h>      // for EC_GROUP_new_by_curve_name, EC_GROUP_free, EC_KEY_new, EC_KEY_set_group, EC_KEY_generate_key, EC_KEY_free
#include <openssl/ecdsa.h>   // for ECDSA_do_sign, ECDSA_do_verify
#include <openssl/obj_mac.h> // for NID_secp192k1
#include <openssl/evp.h>

#include <QCryptographicHash>
#include <QIODevice>
#include <QVector>

namespace QH {

AuthECDSA::AuthECDSA() {

}

QByteArray bignumToArray(const BIGNUM* num) {
    int length = BN_bn2mpi(num, nullptr);
    QVector<unsigned char> data(length);
    BN_bn2mpi(num, data.data());
    QByteArray result;
    result.insert(0, reinterpret_cast<char*>(data.data()), data.length());
    return result;
}

BIGNUM* bignumFromArray(const QByteArray& array) {
    return BN_mpi2bn(reinterpret_cast<const unsigned char*>(array.data()), array.length(), nullptr);
}

QByteArray extractPrivateKey(EC_KEY* ec_key) {
    const BIGNUM* ec_priv = EC_KEY_get0_private_key(ec_key);
    int length = BN_bn2mpi(ec_priv, nullptr);
    QVector<unsigned char> data(length);
    data.insert(0,  length);
    BN_bn2mpi(ec_priv, data.data());
    QByteArray result;
    result.insert(0, reinterpret_cast<char*>(data.data()), data.length());
    return result;
}

QByteArray extractPublicKey(EC_KEY* key, EC_GROUP* group) {

    QByteArray data;
    point_conversion_form_t form = EC_GROUP_get_point_conversion_form(group);

    unsigned char* pub_key_buffer;
    size_t length = EC_KEY_key2buf(key, form, &pub_key_buffer, nullptr);

    if (length <= 0) {
        return {};
    }

    data.insert(0, reinterpret_cast<const char*>(pub_key_buffer), length);

    OPENSSL_free(pub_key_buffer);

    return data;
}

bool AuthECDSA::makeKeys(QByteArray &pubKey, QByteArray &privKey) {

    EC_KEY *eckey= nullptr;
    EC_GROUP *ecgroup = nullptr;


    auto free = [&eckey, &ecgroup] () {
        if (ecgroup)
            EC_GROUP_free(ecgroup);

        if (eckey)
            EC_KEY_free(eckey);
    };

    eckey = EC_KEY_new();
    if (!eckey)
        return false;

    ecgroup = EC_GROUP_new_by_curve_name(NID_secp256k1);

    if (!ecgroup) {
        free();
        return false;
    }

    const int success = 1;
    if ( success != EC_KEY_set_group(eckey, ecgroup)) {
        free();
        return false;
    }

    if ( success != EC_KEY_generate_key(eckey)) {
        free();
        return false;
    }

    pubKey = extractPublicKey(eckey, ecgroup);
    privKey = extractPrivateKey(eckey);

    return pubKey.length() && privKey.length();
}

QByteArray AuthECDSA::signMessage(const QByteArray &inputData,
                                  const QByteArray &key) const {

    auto hash = QCryptographicHash::hash(inputData, QCryptographicHash::Sha256);

    EC_KEY *eckey = EC_KEY_new_by_curve_name(NID_secp256k1);

    BIGNUM* priv = BN_mpi2bn(reinterpret_cast<const unsigned char*>(key.data()),
                             key.length(), nullptr);
    EC_KEY_set_private_key(eckey, priv);
    BN_free(priv);

    ECDSA_SIG *signature = ECDSA_do_sign(reinterpret_cast<const unsigned char*>(hash.data()),
                                         hash.length(), eckey);
    EC_KEY_free(eckey);

    const BIGNUM * R, *S;
    ECDSA_SIG_get0(signature, &R, &S);

    QByteArray result;
    QDataStream stream(&result, QIODevice::WriteOnly);

    stream << bignumToArray(R);
    stream << bignumToArray(S);

    ECDSA_SIG_free(signature);

    return result;
}

bool AuthECDSA::checkSign(const QByteArray &inputData,
                          const QByteArray &signature,
                          const QByteArray &key) const {

    // extract key from raw array;
    EC_KEY *eckey = EC_KEY_new_by_curve_name(NID_secp256k1);
    const EC_GROUP* ec_group = EC_KEY_get0_group(eckey);
    EC_POINT* ec_point = EC_POINT_new(ec_group);
    EC_POINT_oct2point(ec_group, ec_point,
                       reinterpret_cast<const unsigned char*>(key.data()),
                       key.length(), nullptr);
    EC_KEY_set_public_key(eckey, ec_point);
    EC_POINT_free(ec_point);

    // extract signature from raw array

    BIGNUM * R, *S;
    QDataStream stream(signature);

    QByteArray rR,rS;
    stream >> rR;
    stream >> rS;
    R = bignumFromArray(rR);
    S = bignumFromArray(rS);

    ECDSA_SIG *sig = ECDSA_SIG_new();
    ECDSA_SIG_set0(sig, R, S);

    auto hash = QCryptographicHash::hash(inputData, QCryptographicHash::Sha256);

    int verify_status = ECDSA_do_verify(reinterpret_cast<const unsigned char*>(hash.data()),
                                        hash.length(), sig, eckey);

    ECDSA_SIG_free(sig);

    return verify_status == 1;

}

}
