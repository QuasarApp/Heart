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
 * @brief The Abstractkey class is provid one hash function for all keys of Heart library.
 * This class using for generate a hash code of any keyObject.
 */
class HEARTSHARED_EXPORT AbstractKey
{   
public:
    AbstractKey() = default;

    virtual ~AbstractKey() = default;

    /**
     * @brief hash This method must be calc hash int32 code of source object.
     * @return hash 32 bits.
     */
    virtual unsigned int hash() const = 0;

    /**
     * @brief id This method must be return the pointer of id of database object
     * @return BaseId of database object.
     */
    virtual const BaseId* id() const = 0;

    /**
     * @brief table this method must be return the pointer of table name of object
     * @return table name
     */
    virtual const QString *table() const = 0;

    /**
     * @brief isValid This method check this key object to valid
     * @return true if key is valid
     */
    virtual bool isValid() const = 0;
};


}
#endif // ABSTRACTKEY_H
