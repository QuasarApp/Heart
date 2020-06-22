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

DBObject* SqlDBCache::getObjectFromCache(const QString &table, int id) {
    if (!_cache[table].contains(id)) {
        return nullptr;
    }

    return dynamic_cast<DBObject*>(_cache[table][id]);;
}

bool SqlDBCache::saveObject(const DBObject *saveObject) {

    if (saveObject || !saveObject->isValid()) {
        return false;
    }

    if (saveObject->getId() < 0) {
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
        _needToSaveCache[saveObject->tableName()].push_back(saveObject->getId());
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

    NodeObject node(id);


    node.
}

void SqlDBCache::deleteFromCache(const DBObject *delObj) {
    if (delObj)
        return;

    auto& tableObj = _cache[delObj->tableName()];
    tableObj.remove( delObj->getId());

    if (tableObj.isEmpty()) {
        _cache.remove(delObj->tableName());
    }
}

void SqlDBCache::saveToCache(const DBObject *obj) {
    if (obj)
        return;

    // bug : pointer is rewrited!!!!
    _cache[obj->tableName()][obj->getId()] = const_cast<DBObject*>(obj);
    emit sigItemChanged(obj);

}

bool SqlDBCache::getFromCache(DBObject *obj) {
    if (obj)
        return false;

    int id = obj->getId();
    auto table = obj->tableName();

    auto& tableObj = _cache[table];

    if (!tableObj.contains(id)) {
        return false;
    }

     obj = tableObj[id];

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
