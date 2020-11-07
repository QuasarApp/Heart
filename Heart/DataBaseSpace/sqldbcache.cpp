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
#include <execution>


namespace QH {

using namespace PKG;

void SqlDBCache::globalUpdateDataBasePrivate(qint64 currentTime) {

    QMutexLocker lock(&_saveLaterMutex);

    for (uint it : _needToSaveCache) {

        auto async = dynamic_cast<AsyncSqlDbWriter*>(_writer);

        if (_writer && _writer->isValid()) {

            auto obj = getFromCache(it);

            if (!obj || !obj->isValid()) {
                deleteFromCache(obj);

                QuasarAppUtils::Params::log("writeUpdateItemIntoDB failed when"
                                            " db object is not valid! key=" + DESCRIPTION_KEY(it) +
                                            " " + obj->toString(),
                                            QuasarAppUtils::VerboseLvl::Error);
                continue;
            }

            bool saveResult = false;
            if (async)
                saveResult = async->saveObjectWithWait(obj);
            else
                saveResult = _writer->saveObject(obj);



            if (!saveResult ) {
                QuasarAppUtils::Params::log("writeUpdateItemIntoDB failed when"
                                            " work globalUpdateDataRelease!!! key=" + DESCRIPTION_KEY(it) +
                                            " " + obj->toString(),
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

    lastUpdateTime = currentTime;
}

void SqlDBCache::globalUpdateDataBase(SqlDBCasheWriteMode mode) {
    qint64 currentTime = QDateTime::currentMSecsSinceEpoch();

    if (currentTime - lastUpdateTime > updateInterval ||
            static_cast<bool>(mode & SqlDBCasheWriteMode::Force)) {

        if (static_cast<bool>(mode & SqlDBCasheWriteMode::On_New_Thread)) {

            QtConcurrent::run([currentTime, this](){
                globalUpdateDataBasePrivate(currentTime);
            });

        } else {
            globalUpdateDataBasePrivate(currentTime);
        }
    }
}

SqlDBCache::SqlDBCache(qint64 updateInterval, SqlDBCasheWriteMode mode) {
    lastUpdateTime = QDateTime::currentMSecsSinceEpoch();
    this->updateInterval = updateInterval;
    setMode(mode);
}

SqlDBCache::~SqlDBCache() {
    globalUpdateDataBase(SqlDBCasheWriteMode::Force);
}

SqlDBWriter *SqlDBCache::writer() const {
    return _writer;
}

void SqlDBCache::setWriter(SqlDBWriter *writer) {

    if ((_mode == SqlDBCasheWriteMode::Force) && dynamic_cast<AsyncSqlDbWriter*>(writer)) {
        throw std::runtime_error("You user the SqlDBCasheWriteMode::Force mode of SqlDBCache and AsyncSqlDbWriter. "
                                  "This is this can cause your client or server to crash. Since the asynchronous save will be "
                                  "able to be performed for temporary objects. Use the SqlDBCasheWriteMode::Default or"
                                  " not async SqlDbWriter");

    }

    _writer = writer;
}

bool SqlDBCache::getAllObjects(const DBObject &templateObject,  QList<const DBObject *> &result) {

    DBObject* obj = getFromCache(templateObject.dbCacheKey());
    if(obj) {
        result.push_back(obj);
        return true;
    }

    if (_writer && _writer->isValid()) {
        if (!_writer->getAllObjects(templateObject, result)) {
            return false;
        }

        for (auto object: result) {
            if (object->isCached() && !saveToCache(object)) {
                QuasarAppUtils::Params::log("Selected object from database can not be saved into database cache. " +
                                            object->toString(),
                                            QuasarAppUtils::Warning);
            }
        }

        return true;
    }

    return false;
}

bool SqlDBCache::saveObject(const DBObject *saveObject) {

    if (!saveObject || !saveObject->isValid()) {
        return false;
    }

    saveToCache(saveObject);

    if (getMode() == SqlDBCasheWriteMode::Force) {
        if (_writer && _writer->isValid()) {
            if (!_writer->saveObject(saveObject)) {
                return false;
            }

            return true;
        }
    } else {
        _needToSaveCache.insert(saveObject->dbCacheKey());
        globalUpdateDataBase(_mode);
    }

    return true;

}

bool SqlDBCache::deleteObject(const DBObject *delObj) {

    if (!delObj)
        return false;

    deleteFromCache(delObj);

    if (_writer && _writer->isValid()) {
        return _writer->deleteObject(delObj);
    }

    return false;

}

bool SqlDBCache::init(const QString &initDbParams) {

    if (!_writer) {
        return false;
    }

    return _writer->initDb(initDbParams);
}

bool SqlDBCache::init(const QVariantMap &params) {

    if (!_writer) {
        return false;
    }

    return _writer->initDb(params);
}

void SqlDBCache::setSQLSources(const QStringList &list) {
    auto db = writer();
    if (db)
        db->setSQLSources(list);
}

void SqlDBCache::deleteFromCache(const DBObject *delObj) {
    if (!delObj)
        return;

    _cacheMutex.lock();
    unsigned int cacheID = delObj->dbCacheKey();
    unsigned int dbID = delObj->dbKey();

    _cache.remove(cacheID);
    auto siblings = _siblings.value(dbID, nullptr);

    if (siblings) {
        siblings->remove(cacheID);
    }

    _cacheMutex.unlock();
}

bool SqlDBCache::saveToCache(const DBObject *obj) {
    if (!obj)
        return false;

    QMutexLocker lock(&_cacheMutex);
    unsigned int dbAddressKey = obj->dbKey();
    unsigned int dbCacheAddressKey = obj->dbCacheKey();

    auto existsObject = _cache.value(dbCacheAddressKey, nullptr);
    if (!existsObject) {

        _cache[dbCacheAddressKey] = obj->cloneRaw();

    } else if (existsObject->cmd() != obj->cmd()) {

        delete existsObject;
        _cache[dbCacheAddressKey] = obj->cloneRaw();

    } else {

        if (!existsObject->copyFrom(obj)) {
            return false;
        }

    }

    auto siblings = _siblings.value(dbAddressKey, nullptr);

    if (!siblings) {
        siblings = new QHash<uint, const DBObject*>();
        _siblings.insert(dbAddressKey, siblings);
    }

    if (!siblings->contains(dbCacheAddressKey)) {
        siblings->insert(dbCacheAddressKey, obj);
    };

    for (auto sibl: *siblings) {
        emit sigItemChanged(sibl);
    }

    return true;

}

DBObject* SqlDBCache::getFromCache(uint objKey) {

    QMutexLocker locker(&_cacheMutex);

    return _cache.value(objKey, nullptr);
}

SqlDBCasheWriteMode SqlDBCache::getMode() const {
    return _mode;
}

void SqlDBCache::setMode(const SqlDBCasheWriteMode &mode) {
    _mode = mode;
}

qint64 SqlDBCache::getUpdateInterval() const {
    return updateInterval;
}

void SqlDBCache::setUpdateInterval(const qint64 &value) {
    updateInterval = value;
}

}
