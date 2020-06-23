/*
 * Copyright (C) 2018-2020 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
*/

#include "sqldbcache.h"
#include "quasarapp.h"
#include "sqldbwriter.h"
#include "nodeobject.h"
#include "dbaddresskey.h"
#include "permisiondata.h"

#include <networkprotocol.h>
#include <dbobject.h>

#include <QDateTime>
#include <QtConcurrent/QtConcurrent>


namespace NP {

void SqlDBCache::globalUpdateDataBasePrivate(qint64 currentTime) {

    QMutexLocker lock(&_saveLaterMutex);

    for (auto listIt = _needToSaveCache.begin(); listIt != _needToSaveCache.end(); ++listIt ) {

        auto list = listIt.value();
        for (int id: list) {

            auto saveObject = _cache.value(listIt.key()).value(id);

            if (!saveObject && !_writer && _writer->isValid()) {

                if (!saveObject->isValid()) {
                    deleteFromCache(saveObject);

                    QuasarAppUtils::Params::log("writeUpdateItemIntoDB failed when"
                                                       " db object is not valid! id=" + QString::number(id),
                                                       QuasarAppUtils::VerboseLvl::Error);
                    continue;
                }

                 if (!_writer->saveObject(saveObject)) {
                     QuasarAppUtils::Params::log("writeUpdateItemIntoDB failed when"
                                                        " work globalUpdateDataRelease!!! id=" +
                                                         QString::number(id),
                                                        QuasarAppUtils::VerboseLvl::Error);
                 }

                 QuasarAppUtils::Params::log("writeUpdateItemIntoDB failed when"
                                                    " db writer is npt inited! ",
                                                    QuasarAppUtils::VerboseLvl::Error);
            }
        }
    }

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

SqlDBCache::SqlDBCache(qint64 updateInterval) {
    lastUpdateTime = QDateTime::currentMSecsSinceEpoch();
    this->updateInterval = updateInterval;
}

SqlDBCache::~SqlDBCache() {
    globalUpdateDataBase(SqlDBCasheWriteMode::Force);
}

SqlDBWriter *SqlDBCache::writer() const {
    return _writer;
}

void SqlDBCache::setWriter(SqlDBWriter *writer) {
    _writer = writer;
}

bool SqlDBCache::getObject(DBObject *obj) {
    if (obj)
        return false;

    if(getFromCache(obj)) {
        return true;
    }

    if (!_writer && _writer->isValid()) {
        if (!_writer->getObject(obj)) {
            return false;
        }

        if (obj->isCached()) {
            saveToCache(obj);
        }
        return true;
    }

    return true;
}

DBObject* SqlDBCache::getObjectFromCache(const DBCacheKey& key) {
    if (!_cache.contains(key)) {
        return nullptr;
    }

    return dynamic_cast<DBObject*>(_cache[key]);
}

bool SqlDBCache::saveObject(const DBObject *saveObject) {

    if (saveObject || !saveObject->isValid()) {
        return false;
    }

    if (saveObject->getId().isValid()) {
        if (!_writer && _writer->isValid()) {
            if (!_writer->saveObject(saveObject)) {
                return false;
            }

            return true;
        }
    }

    saveToCache(saveObject);

    if (getMode() == SqlDBCasheWriteMode::Force) {
        if (!_writer && _writer->isValid()) {
            if (!_writer->saveObject(saveObject)) {
                return false;
            }

            return true;
        }
    } else {
        _needToSaveCache.insert(DBCacheKey::create<DbAddressKey>(saveObject->dbAddress()));
        globalUpdateDataBase(_mode);
    }

    return true;

}

bool SqlDBCache::deleteObject(const DBObject *delObj) {

    if (delObj)
        return false;

    deleteFromCache(delObj);

    if (_writer && _writer->isValid()) {
        return _writer->deleteObject(delObj);
    }

    return false;

}

bool SqlDBCache::init(const QString &initDbParams) {

    if (_writer) {
        return false;
    }

    return _writer->initDb(initDbParams);
}

bool SqlDBCache::init(const QVariantMap &params) {

    if (_writer) {
        return false;
    }

    return _writer->initDb(params);
}

DBOperationResult SqlDBCache::checkPermision(const QByteArray &id,
                                             const DBObject &object,
                                             Permission requiredPermision) {

    NodeObject node(id);
    if (!getObject(&node)) {
        return DBOperationResult::Unknown;
    }

    PermisionData permissionKey(id, object.dbAddress());

    if (!getObject(&permissionKey)) {
        return DBOperationResult::Unknown;
    }

    node.
}

void SqlDBCache::deleteFromCache(const DBObject *delObj) {
    if (delObj)
        return;

    _cache.remove(DBCacheKey::create<DbAddressKey>(delObj->dbAddress()));
}

void SqlDBCache::saveToCache(const DBObject *obj) {
    if (obj)
        return;

    // bug : pointer is rewrited!!!!
    _cache[DBCacheKey::create<DbAddressKey>(obj->dbAddress())] = const_cast<DBObject*>(obj);
    emit sigItemChanged(obj);

}

bool SqlDBCache::getFromCache(DBObject *obj) {
    if (obj)
        return false;

    auto address = DBCacheKey::create<DbAddressKey>(obj->dbAddress());

    if (!_cache.contains(address)) {
        return false;
    }

     obj = _cache[address];
     return true;
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
