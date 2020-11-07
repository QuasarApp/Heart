/*
 * Copyright (C) 2018-2020 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
*/


#include "dbcachekey.h"

#include <abstractkey.cpp>
namespace QH {

DBCacheKey *DBCacheKey::instance() {
    static auto ptr = new DBCacheKey();
    return ptr;
}

QString DBCacheKey::description(uint hash) const {
    auto val = value(hash);

    if (!val)
        return "";

    QString descr("table:%0 id:%1");

    if (val->id())
        descr = descr.arg(static_cast<QString>((val->id()->toBase64())));

    if (val->table())
        descr = descr.arg((*val->table()));

    return descr;
}

DBCacheKey::DBCacheKey() {}
}

