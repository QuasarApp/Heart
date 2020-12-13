/*
 * Copyright (C) 2018-2020 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
*/

#ifndef SQLITEDBCACHE_H
#define SQLITEDBCACHE_H

#include "sqldbcache.h"

namespace QH {

class sqlitedbcache : public SqlDBCache
{
public:
    sqlitedbcache();
};
}
#endif // SQLITEDBCACHE_H
