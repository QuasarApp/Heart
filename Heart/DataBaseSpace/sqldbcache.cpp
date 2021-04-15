/*
 * Copyright (C) 2018-2021 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
*/

#ifdef HEART_DB_CACHE

#include "sqldbcache.h"
#include "quasarapp.h"
#include "sqldbwriter.h"
#include "dbaddresskey.h"
#include "permisiondata.h"

#include <dbobject.h>
#include <asyncsqldbwriter.h>

#include <QDateTime>
#include <QtConcurrent/QtConcurrent>


namespace QH {

using namespace PKG;

QSharedPointer<DBObject> SqlDBCache::getFromCacheById(quint32 dbKey) {

    QMutexLocker locker(&_cacheMutex);

    return _cache.value(dbKey, nullptr);
}


SqlDBCache::SqlDBCache(qint64 updateInterval):
    ISqlDBCache(updateInterval, SqlDBCasheWriteMode::Force) {

}

SqlDBCache::~SqlDBCache() {

}

void SqlDBCache::deleteFromCache(const QSharedPointer<DBObject>& delObj) {
    if (!delObj)
        return;

    _cacheMutex.lock();
    _cache.remove(delObj->dbKey());
    _cacheMutex.unlock();
}

bool SqlDBCache::insertToCache(const QSharedPointer<DBObject>& obj) {
    if (!obj || !obj->isCached())
        return false;

    // obj must be have valid id of save into cache
    if (!obj->getId().isValid())
        return false;

    QMutexLocker lock(&_cacheMutex);

    if (_cache.contains(obj->dbKey())) {
        return false;
    }

    _cache[obj->dbKey()] = obj->clone();

    return true;

}

bool SqlDBCache::updateCache(const QSharedPointer<DBObject>& obj) {
    if (!obj || !obj->isCached())
        return false;

    QMutexLocker lock(&_cacheMutex);

    auto existsObject = _cache.value(obj->dbKey(), nullptr);

    if (!existsObject) {
        return false;
    }

    if (existsObject->cmd() != obj->cmd()) {
        _cache[obj->dbKey()] = obj->clone();

    } else {
        if (!existsObject->copyFrom(obj.data())) {
            return false;
        }
    }

    return true;
}

QList<QSharedPointer<DBObject> >
SqlDBCache::getFromCache(const DBObject *obj) {

    using resultType = QList<QSharedPointer<QH::PKG::DBObject>>;
    if (obj->getId().isValid()) {
        auto result = getFromCacheById(obj->dbKey());
        if (!result)
            return resultType{};

        return resultType{result};
    }

    return resultType{};
}
}
#endif
