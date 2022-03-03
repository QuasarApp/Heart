/*
 * Copyright (C) 2018-2022 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
*/


#ifndef DBCACHEKEY_H
#define DBCACHEKEY_H

#include <abstractkey.h>
#include <QHash>
#include <heart_global.h>

namespace QH {

/**
 * @brief The DBCacheKey class is memory storage for registered key values.
 * This class have a static array for contains all child classes of the AbstractKey class.
 *
 * Description:
 *
 * Any key it is unsigned int implementation of any AbstractKey child objects.
 * This class provide functions:
 * - The Get of value of AbstractKey child classes (it hash). for more information see the AbstractKey::hash method.
 *   For this use the HASH_KEY(X) macros.
 * - The Calc of hash value of any  AbstractKey child classes (it hash).
 *   If the hash is has been calculated then hash returned from the cache of DBCacheKey.
 *   For this use the VALUE_KEY(X) macros.
 * - The Generate of keys description.
 *   For this use the DESCRIPTION_KEY(X) macros.
 *
 * @note This class is implementation of the Singleton pattern. Use carefully.
 */
class HEARTSHARED_EXPORT DBCacheKey
{
public:

    /**
     * @brief instance This method return instance of this singleton object.
     * @return singleton of object.
     */
    static DBCacheKey* instance();

    template <class TYPE = AbstractKey>
    /**
     * @brief value This method return the value from key. For more convenient use the HASH_KEY(X) macros.
     * @param key This is hash of key value
     * @return value of key.
     */
    const TYPE* value(uint key) const {
        return dynamic_cast<const TYPE*>(_data.value(key, nullptr));
    }

    template <class TYPE>
    /**
     * @brief key This method return hash key and save object into objects table. For more convenient use the VALUE_KEY(X) macros
     * @param value This is value of a key object.
     * @return hash of input value.
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
     * @brief description This method return string description of id (hash). For more convenient use the DESCRIPTION_KEY(X) macros.
     * @param hash This is hash value of object (id).
     * @return information about this hash value.
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
