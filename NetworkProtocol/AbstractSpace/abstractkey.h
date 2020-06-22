/*
 * Copyright (C) 2018-2020 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
*/

#ifndef ABSTRACTKEY_H
#define ABSTRACTKEY_H
#include "networkprotocol_global.h"

namespace NP {

/**
 * @brief The Abstractkey class - this class provid one hash function for all keys of database objcets
 */
class NETWORKPROTOCOLSHARED_EXPORT AbstractKey
{
public:
    AbstractKey();
    virtual ~AbstractKey() = default;
    virtual unsigned int hash() const = 0;
};


}
#endif // ABSTRACTKEY_H
