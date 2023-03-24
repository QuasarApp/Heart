/*
 * Copyright (C) 2021-2023 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
*/


#ifndef ISIGNERDELEGATE_H
#define ISIGNERDELEGATE_H

#include <streambase.h>


namespace QH {

/**
 * @brief The ISignerDelegate class This is base class of signer delegate.
 *  The sisgner class will be invoke all delegate methods for control the input object that need to sign.
 *  @note This class contasin sign and hash fild and override the >> and << operators for QDataStream,
 *  so if you want to convert your object to bytes array don not forget invoke the ISignerDelegate::fromStream and ISignerDelegate::toStream methods.
 * @see Signer
 */
class ISignerDelegate: public StreamBase
{
public:
    ISignerDelegate();

    /**
     * @brief getSignature This method return constant reference to the signature array.
     * @return constant reference to the signature array.
     */
    virtual const QByteArray &getSignature() const;

    /**
     * @brief setSignature This method sets the new signature of this object.
     * @param newSignature new signature value.
     */
    virtual void setSignature(const QByteArray &newSignature);

    /**
     * @brief getHash This method return constant reference to the hash array.
     * @return constant reference to the signature array.
     */
    virtual const QByteArray &getHash() const;

    /**
     * @brief setHash This method sets the new hash sum of this object.
     * @param newSignature new signature value.
     */
    virtual void setHash(const QByteArray &newHash);

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

    // StreamBase interface
protected:
    QDataStream &fromStream(QDataStream &stream) override;
    QDataStream &toStream(QDataStream &stream) const override;

private:
    QByteArray hash;
    QByteArray sign;

};

}
#endif // ISIGNERDELEGATE_H
