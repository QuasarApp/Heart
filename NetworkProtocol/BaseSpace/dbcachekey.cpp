/*
 * Copyright (C) 2018-2020 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
*/


#include "dbcachekey.h"

#include <abstractkey.cpp>
namespace NP {

DBCacheKey *DBCacheKey::instance() {
    static auto ptr = new DBCacheKey();
    return ptr;
}

QString DBCacheKey::description(uint hash) const {
    auto val = value(hash);

    if (!val)
        return "";

    return QString("table:%0 id:%1").arg(val->id().toBase64(), val->table());
}

DBCacheKey::DBCacheKey() {}
}

