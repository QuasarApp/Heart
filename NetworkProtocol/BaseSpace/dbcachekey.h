/*
 * Copyright (C) 2018-2020 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
*/


#ifndef DBCACHEKEY_H
#define DBCACHEKEY_H

#include "baseid.h"

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

    /**
     * @brief DBCacheKey - copy constructor
     * @param other
     */
    DBCacheKey(const DBCacheKey& other) = delete;


    /**
     * @brief DBCacheKey - move constructor
     * @param other
     */
    DBCacheKey(DBCacheKey&& other);

    /**
     * @brief DBCacheKey
     * @param ptr - pointer to value
     * @param fOnltWraper - if this value set be true then destructor of DBCacheKey not remove pointer of value.
     *  by Default this is false.
     */
    DBCacheKey(const AbstractKey *ptr, bool fOnltWraper = false);

    ~DBCacheKey();

    /**
     * @brief castedValue
     * @return casted pointer to data of the object key
     */
    template <class VALUE>
    const VALUE* castedValue() const {
        return dynamic_cast<VALUE*>(this->value());
    }

    /**
     * @brief value
     * @return
     */
    const AbstractKey *value() const;

    /**
     * @brief setValue - set value of key.
     * @param value 0 pointer of value.
     * @param fOnltWraper - if this value set be true then destructor of DBCacheKey not remove pointer of value.
     *  by Default this is false.
     */
    void setValue(const AbstractKey *value, bool fOnlyWraper = false);

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
     * @brief id - return BaseId of object from value. If the value is null then reruen a notValid id.
     * @return BaseId of object
     */
    BaseId id() const;

    /**
     * @brief toString
     * @return string value of this key
     */
    QString toString() const;

    /**
     * @brief isValid -
     * @return
     */
    bool isValid() const;

    friend bool operator !=(const DBCacheKey& left, const DBCacheKey& right);
    friend bool operator ==(const DBCacheKey& left, const DBCacheKey& right);

private:
    const AbstractKey *_value = nullptr;
    bool _onlyWraper = false;
};

uint qHash(const NP::DBCacheKey& key);
}

#endif // DBCACHEKEY_H
