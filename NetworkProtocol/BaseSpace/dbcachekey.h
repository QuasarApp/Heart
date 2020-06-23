/*
 * Copyright (C) 2018-2020 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
*/


#ifndef DBCACHEKEY_H
#define DBCACHEKEY_H

#include "dbid.h"

#include <networkprotocol_global.h>

namespace NP {

class AbstractKey;

/**
 * @brief The DBCacheKey class - is database main key value
 */
class NETWORKPROTOCOLSHARED_EXPORT DBCacheKey
{
public:
    DBCacheKey();
    DBCacheKey(AbstractKey *ptr);

    ~DBCacheKey();

    /**
     * @brief castedValue
     * @return casted pointer to data of the object key
     */
    template <class VALUE>
    const VALUE* castedValue() const {
        return dynamic_cast<VALUE*>(this->value());
    }

    AbstractKey *value() const;
    void setValue(AbstractKey *value);

    /**
     * @brief create - create key from value
     * @param value - value
     * @return key
     */
    template <class KeyType, class VALUE>
    static DBCacheKey create(const VALUE& value) {
        return DBCacheKey(new KeyType(value));
    }

    /**
     * @brief table - return the table name from value of key. If the value is null then return a empty string
     * @return name of table of object
     */
    QString table() const;

    /**
     * @brief id - return dbId of object from value. If the value is null then reruen a notValid id.
     * @return dbid of object
     */
    DbId id() const;

private:
    AbstractKey * _value = nullptr;
};

uint hash(const DBCacheKey& key);
}
#endif // DBCACHEKEY_H
