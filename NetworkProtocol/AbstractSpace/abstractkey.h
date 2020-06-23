/*
 * Copyright (C) 2018-2020 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
*/

#ifndef ABSTRACTKEY_H
#define ABSTRACTKEY_H
#include "networkprotocol_global.h"

#include <dbid.h>

namespace NP {

/**
 * @brief The Abstractkey class - this class provid one hash function for all keys of database objcets
 */
class NETWORKPROTOCOLSHARED_EXPORT AbstractKey
{
public:
    AbstractKey();

    /**
     * @brief AbstractKey
     * @param cusomHash - custom hash
     */
    AbstractKey(unsigned int cusomHash);

    virtual ~AbstractKey() = default;
    virtual unsigned int hash() const;

    /**
     * @brief id - this method return id of object
     * @return dbid of object.
     */
    virtual const DbId& id() const = 0;

    /**
     * @brief table - this method return table name of object
     * @return table name
     */
    virtual const QString& table() const = 0;

    /**
     * @brief setCustomHash - set custom hash for key. if custom hash is 0 then hash function return hash from objrct
     * @param customHash
     */
    void setCustomHash(unsigned int customHash);

private:
    unsigned int _customHash = 0;
};


}
#endif // ABSTRACTKEY_H
