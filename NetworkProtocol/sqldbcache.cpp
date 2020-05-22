/*
 * Copyright (C) 2018-2020 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
*/

#include "sqldbcache.h"
#include "quasarapp.h"
#include "sqldbwriter.h"

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

            if (!saveObject.isNull() && !_writer.isNull() && _writer->isValid()) {

                if (!saveObject->isValid()) {
                    deleteFromCache(listIt.key(), id);

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

WP<SqlDBWriter> SqlDBCache::writer() const {
    return _writer;
}

void SqlDBCache::setWriter(const WP<SqlDBWriter> &writer) {
    _writer = writer;
}

bool SqlDBCache::getObject(SP<DBObject> &obj) {
    if (obj.isNull())
        return false;

    int id = obj->getId();
    auto table = obj->tableName();

    auto& tableObj = _cache[table];

    if (!tableObj.contains(id) && !_writer.isNull() && _writer->isValid()) {
        if (!_writer->getObject(obj)) {
            return false;
        }

        saveToCache(obj);
        return true;
    }

    auto &sptr = tableObj[id];

    if (!sptr->isCached() && _writer->getObject(sptr)) {
        saveToCache(sptr);
    }

    if (!sptr->isValid()) {
        deleteFromCache(table, id);
        return false;
    }

    obj = sptr;
    return true;
}

SP<DBObject> &SqlDBCache::getObjectFromCache(const QString &table, int id) {
    auto& tableObj = _cache[table];
    return tableObj[id];
}

bool SqlDBCache::saveObject(const WP<AbstractData>& saveObject) {

    auto ptr = saveObject.toStrongRef().dynamicCast<DBObject>();

    if (ptr.isNull() || !ptr->isValid()) {
        return false;
    }

    saveToCache(ptr);

    if (getMode() == SqlDBCasheWriteMode::Force) {
        if (!_writer.isNull() && _writer->isValid()) {
            if (!_writer->saveObject(saveObject)) {
                return false;
            }

            return true;
        }
    } else {
        _needToSaveCache[ptr->tableName()].push_back(ptr->getId());
        globalUpdateDataBase(_mode);
    }

    return true;

}

bool SqlDBCache::deleteObject(const WP<AbstractData> &delObj) {
    auto ref = delObj.toStrongRef().dynamicCast<DBObject>();

    if (ref.isNull())
        return false;

    deleteFromCache(ref->tableName(), ref->getId());

    if (_writer && _writer->isValid()) {
        return _writer->deleteObject(delObj);
    }

    return false;

}

bool SqlDBCache::init(const QString &initDbParams) {

    if (_writer.isNull()) {
        return false;
    }

    return _writer->initDb(initDbParams);
}

bool SqlDBCache::init(const QVariantMap &params) {

    if (_writer.isNull()) {
        return false;
    }

    return _writer->initDb(params);
}

void SqlDBCache::deleteFromCache(const QString &table, int id) {
    auto& tableObj = _cache[table];
    tableObj.remove(id);

    if (tableObj.isEmpty()) {
        _cache.remove(table);
    }
}

void SqlDBCache::saveToCache(const WP<AbstractData> &obj) {

    auto ref = obj.toStrongRef().dynamicCast<DBObject>();

    if (ref.isNull())
        return;

    // bug : pointer is rewrited!!!!
    _cache[ref->tableName()][ref->getId()] = ref;
    emit sigItemChanged(obj);

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
