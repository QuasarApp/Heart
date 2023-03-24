/*
 * Copyright (C) 2021-2023 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
*/


#ifndef SIGNER_H
#define SIGNER_H

#include <QByteArray>
#include <QCryptographicHash>
#include <QSharedPointer>
#include "isignerdelegate.h"
#include "qaservice.h"

namespace QH {

/**
 * @brief The Signer class can sign and check the signature of the any childs classes of the ISignerDelegate class.
 *
 * **Exmaple of use**
 *
 * @code{cpp}
 * #include <hcriptoFeatures/isignerdelegate.h>
 * #include <hcriptoFeatures/ecdsasigner.h>
 *
 * class MyDelegate: public ISignerDelegate {
 *  // override all methods
 * };
 *
 * int main () {
 *     MyDelegate delegate;
 *
 *     // sign your object
 *     QH::ECDSASigner::sign(&delegate);
 *
 *     // check signature of the object
 *     QH::ECDSASigner::check(&delegate);
 * }
 *
 *
 * @endcode
 *
 * @note the MyDelegate class should be contains a public for check signature and a private for sign object keys
 * @tparam CryptoImplementation This is internal implementaion of base encription functions.
 * @see iCrypto class.
 */
template<class CryptoImplementation>
class Signer: protected QuasarAppUtils::Service<Signer<CryptoImplementation>>,
              protected CryptoImplementation
{
public:
    Signer() {};
    ~Signer() {};

    /**
     * @brief sign This is main method for signing of this object.
     * @return true if the object signed sucessful
     */
    static bool sign(ISignerDelegate* obj) {

        if (auto signer = Signer<CryptoImplementation>::autoInstance()) {
            auto sign = signer->signMessage(obj->getMessage(), obj->getPrivateKey());

            if (sign.size()) {
                obj->setSignature(sign);
                return true;
            }

        }

        return false;
    };

    /**
     * @brief check This method check signature of this object.
     * @return true if the objec signed
     */
    static bool check(const ISignerDelegate* obj) {
        if (auto signer = Signer<CryptoImplementation>::autoInstance()) {
            return signer->checkSign(obj->getMessage(), obj->signature(), obj->getPublicKey());
        }

        return false;
    };

};
}
#endif // SIGNER_H
