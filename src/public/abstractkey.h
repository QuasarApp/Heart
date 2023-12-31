/*
 * Copyright (C) 2018-2024 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
*/

#ifndef ABSTRACTKEY_H
#define ABSTRACTKEY_H
#include "heart_global.h"

namespace QH {

/**
 * @brief The Abstractkey class is provide general hash function for all keys of Heart library.
 * This class using for generate a hash code of any keyObject.
 */
class HEARTSHARED_EXPORT AbstractKey
{   
public:
    AbstractKey() = default;

    virtual ~AbstractKey() = default;

    /**
     * @brief hash This method must be calc hash int32 code of source object.
     * @return Hash 32 bits.
     */
    virtual unsigned int hash() const = 0;

    /**
     * @brief equal This method must be make compare between current object and other.
     * @param other This is other comparable object.
     * @return True if objects is equals.
     */
    virtual bool equal(const AbstractKey* other) const = 0;

    /**
     * @brief toString This method should be return the string implementation of this key.
     * @return String value.
     */
    virtual QString toString() const = 0;

    /**
     * @brief isValid This method check this key object to valid.
     * @return True if key is valid.
     */
    virtual bool isValid() const = 0;
};


}
#endif // ABSTRACTKEY_H
