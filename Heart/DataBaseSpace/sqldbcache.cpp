/*
 * Copyright (C) 2018-2020 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
*/

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

void SqlDBCache::globalUpdateDataBasePrivate(qint64 currentTime) {

    QMutexLocker lock(&_saveLaterMutex);

    for (auto it = _needToSaveCache.begin(); it != _needToSaveCache.end(); ++it) {

        if (writer() && writer()->isValid()) {

            auto obj = getFromCache(it.value());

            if (!obj || !obj->isValid()) {
                deleteFromCache(obj);

                QuasarAppUtils::Params::log("writeUpdateItemIntoDB failed when"
                                            " db object is not valid! obj=" +
                                            obj->toString(),
                                            QuasarAppUtils::VerboseLvl::Error);
                continue;
            }

            bool saveResult = false;

            if (it.key() == MemberType::Insert) {
                    saveResult = writer()->insertObject(obj, true);
            } else {
                    saveResult = writer()->updateObject(obj, true);
            }

            if (!saveResult ) {
                QuasarAppUtils::Params::log("writeUpdateItemIntoDB failed when"
                                            " work globalUpdateDataRelease!!! obj=" +
                                            obj->toString(),
                                            QuasarAppUtils::VerboseLvl::Error);
            }
        } else {

            QuasarAppUtils::Params::log("writeUpdateItemIntoDB failed when"
                                        " db writer is npt inited! ",
                                        QuasarAppUtils::VerboseLvl::Error);
            return;
        }
    }

    _needToSaveCache.clear();
    setLastUpdateTime(currentTime);
}

void SqlDBCache::pushToQueue(const DBObject *obj, MemberType type) {
    _saveLaterMutex.lock();
    _needToSaveCache.insert(type, obj);
    _saveLaterMutex.unlock();
}


SqlDBCache::SqlDBCache(qint64 updateInterval, SqlDBCasheWriteMode mode):
    ISqlDBCache(updateInterval, mode) {

}

SqlDBCache::~SqlDBCache() {

}

void SqlDBCache::deleteFromCache(const DBObject *delObj) {
    if (!delObj)
        return;

    _cacheMutex.lock();
    _cache.remove(delObj->dbKey());
    _cacheMutex.unlock();
}

bool SqlDBCache::insertToCache(const DBObject *obj) {
    if (!obj)
        return false;

    // obj must be have valid id of save into cache
    if (!obj->getId().isValid())
        return false;

    QMutexLocker lock(&_cacheMutex);

    if (_cache.contains(obj->dbKey())) {
        return false;
    }

    _cache[obj->dbKey()] = obj->cloneRaw();

    emit sigItemChanged(obj);

    return true;

}

bool SqlDBCache::updateCache(const DBObject *obj) {
    if (!obj)
        return false;

    QMutexLocker lock(&_cacheMutex);

    auto existsObject = _cache.value(obj->dbKey(), nullptr);

    if (!existsObject) {
        return false;
    }

    if (existsObject->cmd() != obj->cmd()) {

        delete existsObject;
        _cache[obj->dbKey()] = obj->cloneRaw();

    } else {
        if (!existsObject->copyFrom(obj)) {
            return false;
        }
    }

    emit sigItemChanged(obj);

    return true;
}

DBObject* SqlDBCache::getFromCache(const DBObject *obj) {

    QMutexLocker locker(&_cacheMutex);

    return _cache.value(obj->dbKey(), nullptr);
}
}
