/*
 * Copyright (C) 2021-2022 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
*/


#ifndef SIGNER_H
#define SIGNER_H

#include <QByteArray>
#include <QCryptographicHash>

namespace QH {

/**
 * @brief The Signer class is base wraper class for sining objects
 * This class implement simple verification of the object with public and private keys and do not use any certificates.
 * @tparam CryptoImplementation This is internal implementaion of base encription functions.
 * @see iCrypto class.
 */
template<class CryptoImplementation>
class Signer: public CryptoImplementation
{
public:
    Signer() {};
    ~Signer() {};

    /**
     * @brief signature This method should be return constant reference to the signature array.
     * @return constant reference to the signature array.
     */
    virtual const QByteArray &signature() const = 0;

    /**
     * @brief setSignature This method should be set the new signature of this object.
     * @param newSignature new signature value.
     */
    virtual void setSignature(const QByteArray &newSignature) = 0;

    /**
     * @brief signMessage This is main method for signing of this object.
     * @return true if the object signed sucessful
     */
    bool signMessage() const override {
        auto sign = CryptoImplementation::signMessage(getMessage(), getPrivateKey());

        if (sign.size()) {
            setSignature(sign);
            return true;
        }

        return false;
    };

    /**
     * @brief checkSign This method check signatyre of this object.
     * @return true if the objec signed
     */
    bool checkSign() const override {
        return CryptoImplementation::checkSign(getMessage(), signature(), getPublicKey());
    };


protected:

    /**
     * @brief getPrivateKey This method should be return private key for the public key that saved in this object.
     * @return private key for the public key that saved in this object.
     */
    virtual const QByteArray& getPrivateKey() const = 0;

    /**
     * @brief getPublicKey This method should be return public key for the private key that encrypted this object.
     * @return public key for the private key that ecrypt this object.
     */
    virtual const QByteArray& getPublicKey() const = 0;

    /**
     * @brief getMessage This method should be return message that you want to sign.
     * @return message that you want to sign.
     */
    virtual const QByteArray& getMessage() const = 0;

};
}
#endif // SIGNER_H
