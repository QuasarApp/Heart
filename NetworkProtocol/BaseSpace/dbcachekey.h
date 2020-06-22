/*
 * Copyright (C) 2018-2020 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
*/


#ifndef DBCACHEKEY_H
#define DBCACHEKEY_H

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
    ~DBCacheKey();

    AbstractKey *value() const;
    void setValue(AbstractKey *value);

private:
    AbstractKey * _value = nullptr;
};

uint hash(const DBCacheKey& key);
}
#endif // DBCACHEKEY_H
