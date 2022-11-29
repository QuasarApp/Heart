/*
 * Copyright (C) 2018-2022 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
*/


#ifndef KEYSTORAGE_TEST_H
#define KEYSTORAGE_TEST_H

#include <keystorage.h>
#include <cryptopairkeys.h>

/**
 * This test check working the crypto module of the QH::HEART
 */
template<class Crypto>
bool validationCrypto() {
    // create crypto oject
    auto crypto = new QH::KeyStorage(new Crypto());


    // get test pair keys
    auto keys = crypto->getNextPair("TEST_KEY");

    // must be failed becouse crypto object still not inited.
    if (keys.isValid()) {
        delete crypto;
        return false;
    }

    if (!crypto->initDefaultStorageLocation()) {
        delete crypto;
        return false;
    }

    // get test pair keys
    keys = crypto->getNextPair("TEST_KEY");

    // chekck keys
    if (!keys.isValid()) {
        delete crypto;
        return false;
    }

    // remove crypto object, after remove crypto object most be save all generated keys
    delete crypto;

    // second initialisin of crypto object
    crypto = new QH::KeyStorage(new Crypto());
    if (!crypto->initDefaultStorageLocation()) {
        delete crypto;
        return false;
    }

    // check get generated key pair
    if (keys != crypto->getNextPair("TEST_KEY", 0)) {
        delete crypto;
        return false;
    }

    QByteArray msg = "test_message";

    // check sign data
    if (!crypto->sign(&msg, keys.privKey())) {
        delete crypto;
        return false;
    }

    if (!crypto->check(msg, keys.publicKey())) {
        delete crypto;
        return false;
    }

    // check genesis generation of keys
    auto ThisIsKey = crypto->getNextPair("key", "this is key");
    auto ThisIsKey2 = crypto->getNextPair("key2", "this is key");

    if (ThisIsKey != ThisIsKey2) {
        delete crypto;
        return false;
    }


    delete crypto;

    crypto = new QH::KeyStorage(new Crypto());
    if (!crypto->initDefaultStorageLocation()) {
        delete crypto;
        return false;
    }

    auto lastKeys = crypto->getNextPair("key2", RAND_KEY, 0);
    return lastKeys == ThisIsKey2;
}

#endif // KEYSTORAGE_TEST_H
