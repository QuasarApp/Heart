/*
 * Copyright (C) 2018-2020 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
*/

#ifndef ABSTRACTKEY_H
#define ABSTRACTKEY_H
#include "heart_global.h"

#include <baseid.h>

namespace QH {

/**
 * @brief The Abstractkey class - this class provid one hash function for all keys of database objcets
 */
class NETWORKPROTOCOLSHARED_EXPORT AbstractKey
{   
public:
    AbstractKey() = default;

    virtual ~AbstractKey() = default;
    virtual unsigned int hash() const = 0;
    virtual bool equal(const AbstractKey* other) const = 0;

    /**
     * @brief id - this method return id of object
     * @return BaseId of object.
     */
    virtual const BaseId& id() const = 0;

    /**
     * @brief table - this method return table name of object
     * @return table name
     */
    virtual const QString& table() const = 0;

    /**
     * @brief isValid
     * @return true if key is valid
     */
    virtual bool isValid() const = 0;
};


}
#endif // ABSTRACTKEY_H
