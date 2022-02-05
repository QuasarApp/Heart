/*
 * Copyright (C) 2021-2021 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
*/

#ifndef ASYNCKEYSAUTH_H
#define ASYNCKEYSAUTH_H

#include <QByteArray>

#include "heart_global.h"

namespace QH {

/**
 * @brief The AsyncKeysAuth class is base class for works with authorization of a pair of asynchronous keys
 *
 * ### How to it works:
 *
 * * -----
 *
 * ** Client Part **
 *
 * * Client make a pair keys (public and private) = PUB and PRIV
 * * Client get current unix time = U
 * * Client make a data for signing = S
 *      S = SHA256{U + PUB}
 * * Client make a signature SIG
 *      SIG = PRIV.encript(S)
 * * Client prepare a auth request for server = R:
 *      R = {U,SIG,PUB}
 * * Cleint send R to server
 *
 * * -----
 *
 * ** Server Part **
 *
 * * Server receive R from client.
 * * Server compare U time with current unix time.
 * * If the diferrence more then allowed value then server reject an auth
 * * Server make S value as a client
 * * Server decript SIG value and comapre it with S value
 * * If decripted value is deferrend of the S then server reject an auth.
 * * If decripted value equals with S value then server accept an auth.
 * * After accept server create new user with ID = sha256(PUB) or
 * if user alredy exits make them as a logined user.
 *
 *
 */
class HEARTSHARED_EXPORT AsyncKeysAuth
{
public:
    AsyncKeysAuth();

    /**
     * @brief auth This method make authentication and return true if the authentication finished successful else false.
     * @return true if the authentication finished successful else false.
     */
    bool auth(int allowedTimeRangeSec) const;

    /**
     * @brief prepare This method will generate signature for autentication of client. Please inboke this method before send request to server.
     * @return true if signature generated sucessuful.
     */
    bool prepare();

    /**
     * @brief unixTime This method return unix time that client added for authentication.
     * @return unix time that client added for authentication.
     * @see AsyncKeysAuth::setUnixTime
     */
    unsigned int unixTime() const;

    /**
     * @brief setUnixTime This method sets new value of the unixTime propertye.
     * @param newUnixTime This is new unix time value. Unix time sets in secunds from 1970 year
     */
    void setUnixTime(unsigned int newUnixTime);

    /**
     * @brief signature This method return signature array.
     * @return signature array.
     * @see AsyncKeysAuth::setSignature
     */
    const QByteArray &signature() const;

    /**
     * @brief publicKey This method return public key that client added for authentication.
     * @note The @a publicKey will be used forcreate user id.
     * @return public key that client added for authentication.
     * @see AsyncKeysAuth::setPublicKey
     */
    const QByteArray &publicKey() const;

    /**
     * @brief setPublicKey This method sets new public key for authentication.
     * @param newPublicKey Thiy is new key.
     * @see AsyncKeysAuth::publicKey
     */
    void setPublicKey(const QByteArray &newPublicKey);

    /**
     * @brief isValid this method check this ibject to valid.
     * @return return true if object contains valid signature else false.
     * @note Invoke the AsyncKeysAuth::prepare method before check valid of object. All object that not be preparred is invalid.
     */
    bool isValid() const;

protected:

    /**
     * @brief encrypt This method should be encript the @a inputData using the @a key.
     * @param inputData This is input data that should be encripted.
     * @param key This is a privete key for encription the @a inpputData.
     * @return encripted data array.
     * @see AsyncKeysAuth::descrupt
     */
    virtual QByteArray encrypt(const QByteArray& inputData, const QByteArray& key) const = 0;

    /**
     * @brief decrypt This method should be decrypt the @a inputData using the @a key.
     * @param inputData This is input data that should be decripted.
     * @param key This is a public key for encription the @a inpputData.
     * @return decripted data array.
     * @see AsyncKeysAuth::encrypt
     */
    virtual QByteArray decrypt(const QByteArray& inputData, const QByteArray& key) const = 0;

    /**
     * @brief getPrivateKey This method should be return private key for the public key that saved in this object.
     * @return private key for the public key that saved in this object.
     */
    virtual QByteArray getPrivateKey() const = 0;

    /**
     * @brief setSignature Tihis is internal method for sets new signature value.
     * @param newSignature new signature value.
     * @note used in the
     */
    void setSignature(const QByteArray &newSignature);

private:
    unsigned int _unixTime = 0;
    QByteArray _signature;
    QByteArray _publicKey;
};

}

#endif // ASYNCKEYSAUTH_H
