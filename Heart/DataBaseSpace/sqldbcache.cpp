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

    for (auto it = _needToSaveCache.begin(); it != _needToSaveCache.end(); ++it) {

        auto async = dynamic_cast<AsyncSqlDbWriter*>(_writer);

        if (_writer && _writer->isValid()) {

            auto obj = getFromCache(it.value());

            if (!obj || !obj->isValid()) {
                deleteFromCache(obj);

                QuasarAppUtils::Params::log("writeUpdateItemIntoDB failed when"
                                            " db object is not valid! key=" +
                                            DESCRIPTION_KEY(it.value()) +
                                            " " + obj->toString(),
                                            QuasarAppUtils::VerboseLvl::Error);
                continue;
            }

            bool saveResult = false;

            if (it.key() == MemberType::Insert) {
                if (async)
                    saveResult = async->insertObjectWithWait(obj);
                else
                    saveResult = _writer->insertObject(obj);
            } else {
                if (async)
                    saveResult = async->updateObjectWithWait(obj);
                else
                    saveResult = _writer->updateObject(obj);
            }

            if (!saveResult ) {
                QuasarAppUtils::Params::log("writeUpdateItemIntoDB failed when"
                                            " work globalUpdateDataRelease!!! key=" +
                                            DESCRIPTION_KEY(it.value()) +
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

    DBObject* obj = getFromCache(templateObject.dbKey());
    if(obj) {
        result.push_back(obj);
        return true;
    }

    if (_writer && _writer->isValid()) {
        if (!_writer->getAllObjects(templateObject, result)) {
            return false;
        }

        for (auto object: result) {
            if (object->isCached() && !insertToCache(object)) {
                QuasarAppUtils::Params::log("Selected object from database can not be saved into database cache. " +
                                            object->toString(),
                                            QuasarAppUtils::Warning);
            }
        }

        return true;
    }

    return false;
}

bool SqlDBCache::updateObject(const DBObject *saveObject) {

    if (!saveObject || !saveObject->isValid()) {
        return false;
    }

    if (!updateCache(saveObject)) {
        return false;
    }

    if (getMode() == SqlDBCasheWriteMode::Force) {
        if (_writer && _writer->isValid()) {
            if (!_writer->updateObject(saveObject)) {
                return false;
            }

            return true;
        }
    } else {
        _needToSaveCache.insert(MemberType::Update, saveObject->dbKey());
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

bool SqlDBCache::insertObject(const DBObject *saveObject) {
    if (!saveObject || !saveObject->isValid()) {
        return false;
    }

    if (!insertObject(saveObject)) {
        return false;
    }

    if (getMode() == SqlDBCasheWriteMode::Force) {
        if (_writer && _writer->isValid()) {
            if (!_writer->insertObject(saveObject)) {
                return false;
            }

            return true;
        }
    } else {
        _needToSaveCache.insert(MemberType::Insert, saveObject->dbKey());
        globalUpdateDataBase(_mode);
    }

    return true;
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
    _cache.remove(delObj->dbKey());
    _cacheMutex.unlock();
}

bool SqlDBCache::insertToCache(const DBObject *obj) {
    if (!obj)
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
