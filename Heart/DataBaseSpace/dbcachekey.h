/*
 * Copyright (C) 2018-2020 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
*/


#ifndef DBCACHEKEY_H
#define DBCACHEKEY_H

#include "baseid.h"

#include <abstractkey.h>
#include <QHash>
#include <heart_global.h>

namespace NP {

/**
 * @brief The DBCacheKey class - is database main key value
 */
class NETWORKPROTOCOLSHARED_EXPORT DBCacheKey
{
public:

    /**
     * @brief instance
     * @return singleton of object
     */
    static DBCacheKey* instance();

    template <class TYPE = AbstractKey>
    /**
     * @brief value - return vale from key
     * @param key - hash of ke value
     * @return value of key
     */
    const TYPE* value(uint key) const {
        return dynamic_cast<const TYPE*>(_data.value(key, nullptr));
    }

    template <class TYPE>
    /**
     * @brief key - return hash key and save object into objects table
     * @param value - the value of a key objekt
     * @return hash of input value
     */
    uint key(const TYPE& value) {
        auto object = dynamic_cast<const AbstractKey*>(&value);

        if (!object) {
            return 0;
        }

        uint hash = object->hash();
        if (_data.contains(hash)) {
            _data[hash] = new TYPE(value);
        }
        return hash;
    }

    /**
     * @brief description - return string description of id
     * @param hash
     * @return
     */
    QString description(uint hash) const;


private:
    QHash<uint, AbstractKey*> _data;
    DBCacheKey();

};

#define HASH_KEY(X) DBCacheKey::instance()->key(X)
#define VALUE_KEY(X) DBCacheKey::instance()->value(X)
#define DESCRIPTION_KEY(X) DBCacheKey::instance()->description(X)

}

#endif // DBCACHEKEY_H
