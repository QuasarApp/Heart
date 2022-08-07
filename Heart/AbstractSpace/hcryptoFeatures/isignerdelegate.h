/*
 * Copyright (C) 2021-2022 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
*/


#ifndef ISIGNERDELEGATE_H
#define ISIGNERDELEGATE_H

#include <QByteArray>


namespace QH {

/**
 * @brief The ISignerDelegate class This is base interface of signer delegate.
 *  THe sisgner class will be invoke all dlegate mthods for control the input object that need to sign.
 * @see Signer
 */
class ISignerDelegate
{
public:
    ISignerDelegate();

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
    virtual const QByteArray getMessage() const = 0;
};

}
#endif // ISIGNERDELEGATE_H
