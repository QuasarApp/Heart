/*
 * Copyright (C) 2018-2023 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
*/

#include "isqldb.h"
#include "sqldbwriter.h"

#include <dbobject.h>
#include <asyncsqldbwriter.h>

#include <QDateTime>
#include <QtConcurrent/QtConcurrent>
#include <qaglobalutils.h>

namespace QH {

using namespace PKG;

void ISqlDB::globalUpdateDataBase(SqlDBCasheWriteMode mode) {
    qint64 currentTime = QDateTime::currentMSecsSinceEpoch();

    if (currentTime - lastUpdateTime > updateInterval ||
            static_cast<bool>(mode & SqlDBCasheWriteMode::Force)) {

        if (static_cast<bool>(mode & SqlDBCasheWriteMode::On_New_Thread)) {

#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
            QtConcurrent::run([currentTime, this]() {
                globalUpdateDataBasePrivate(currentTime);
            });

#else
            auto future = QtConcurrent::run([currentTime, this]() {
                globalUpdateDataBasePrivate(currentTime);
            });

            if (!future.isValid()) {
                QuasarAppUtils::Params::log("Failde to start update database thread");
            }
#endif
        } else {
            globalUpdateDataBasePrivate(currentTime);
        }
    }
}

bool ISqlDB::updateObjectP(const QSharedPointer<DBObject> &saveObject,
                                bool wait) {

    if (updateCache(saveObject)) {

        if (getMode() == SqlDBCasheWriteMode::Force) {
            return _writer && _writer->isValid() &&
                    _writer->updateObject(saveObject, wait);
        }

        pushToQueue(saveObject, CacheAction::Update);
        globalUpdateDataBase(getMode());

        return true;
    }

    return  _writer && _writer->isValid() &&
            _writer->updateObject(saveObject, wait);
}

bool ISqlDB::deleteObjectP(const QSharedPointer<DBObject> &delObj,
                                bool wait) {

    deleteFromCache(delObj);
    pushToQueue(delObj, CacheAction::Delete);

    if (_writer && _writer->isValid()) {
        return _writer->deleteObject(delObj, wait);
    }

    return false;
}

bool ISqlDB::insertObjectP(const QSharedPointer<DBObject> &saveObject,
                                bool wait) {

    if (insertToCache(saveObject)) {

        if (getMode() == SqlDBCasheWriteMode::Force) {

            return _writer && _writer->isValid() &&
                    _writer->insertObject(saveObject, wait);
        }

        pushToQueue(saveObject, CacheAction::Update);
        globalUpdateDataBase(getMode());

        return true;
    }

    return _writer && _writer->isValid() &&
           _writer->insertObject(saveObject, wait);
}

bool ISqlDB::replaceObjectP(const QSharedPointer<PKG::DBObject> &saveObject, bool wait) {
    if (updateCache(saveObject)) {

        if (getMode() == SqlDBCasheWriteMode::Force) {

            return _writer && _writer->isValid() &&
                   _writer->replaceObject(saveObject, wait);
        }

        pushToQueue(saveObject, CacheAction::Update);
        globalUpdateDataBase(getMode());

        return true;
    }

    return _writer && _writer->isValid() &&
           _writer->replaceObject(saveObject, wait);
}

qint64 ISqlDB::getLastUpdateTime() const {
    return lastUpdateTime;
}

void ISqlDB::setLastUpdateTime(const qint64 &value) {
    lastUpdateTime = value;
}

void ISqlDB::pushToQueue(const QSharedPointer<DBObject> &obj,
                              CacheAction type) {
    _saveLaterMutex.lock();
    _changes.insert(type, obj);
    _saveLaterMutex.unlock();
}

ISqlDB::ISqlDB(qint64 updateInterval, SqlDBCasheWriteMode mode) {
    lastUpdateTime = QDateTime::currentMSecsSinceEpoch();
    this->updateInterval = updateInterval;
    setMode(mode);

}

ISqlDB::~ISqlDB() {

}

SqlDBWriter *ISqlDB::writer() const {
    return _writer;
}

void ISqlDB::setWriter(SqlDBWriter *writer) {
    _writer = writer;
}

bool ISqlDB::getAllObjects(const DBObject &templateObject,
                                QList<QSharedPointer<QH::PKG::DBObject>> &result) {

    result = getFromCache(&templateObject);
    if(result.size()) {
        return true;
    }

    if (_writer && _writer->isValid()) {
        if (!_writer->getAllObjects(templateObject, result)) {
            return false;
        }

        for (const auto &object: qAsConst(result)) {
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

bool ISqlDB::deleteObject(const QSharedPointer<DBObject> &delObj,
                               bool wait) {

    if (!delObj)
        return false;

    auto id = delObj->dbAddress();

    if (!deleteObjectP(delObj, wait)) {
        return false;
    }

    if (id.isValid())
        emit sigItemDeleted(id);

    return true;

}

bool ISqlDB::updateObject(const QSharedPointer<DBObject> &saveObject,
                               bool wait) {

    if (!saveObject || !saveObject->isValid()) {
        return false;
    }

    if (!updateObjectP(saveObject, wait)) {
        return false;
    }

    emit sigItemChanged(saveObject);

    return true;
}

bool ISqlDB::insertObject(const QSharedPointer<DBObject> &saveObject, bool wait) {
    if (!saveObject || !saveObject->isValid()) {
        return false;
    }

    if (!insertObjectP(saveObject, wait)) {
        return false;
    }

    emit sigItemChanged(saveObject);

    return true;
}

bool ISqlDB::replaceObject(const QSharedPointer<PKG::DBObject> &saveObject, bool wait) {
    if (!saveObject || !saveObject->isValid()) {
        return false;
    }

    if (!replaceObjectP(saveObject, wait)) {
        return false;
    }

    emit sigItemChanged(saveObject);

    return true;
}

bool ISqlDB::doQuery(const QString &query, bool wait,
                          QSqlQuery *result) const {

    if (!_writer) {
        return false;
    }

    return _writer->doQuery(query, wait, result);
}

bool ISqlDB::doSql(const QString &sqlFile, bool wait) const {
    if (!_writer) {
        return false;
    }

    return _writer->doSql(sqlFile, wait);
}

bool ISqlDB::init(const QString &initDbParams) {

    if (!_writer) {
        return false;
    }

    return _writer->initDb(initDbParams);
}

bool ISqlDB::init(const QVariantMap &params) {

    if (!_writer) {
        return false;
    }

    return _writer->initDb(params);
}

void ISqlDB::setSQLSources(const QStringList &list) {
    auto db = writer();
    if (db)
        db->setSQLSources(list);
}

void ISqlDB::prepareForDelete() {
    globalUpdateDataBase(SqlDBCasheWriteMode::Force);
}

bool ISqlDB::changeObjects(const DBObject &templateObject,
                                const std::function<bool (const QSharedPointer<QH::PKG::DBObject>&)> &changeAction) {

    QList<QSharedPointer<DBObject>> list;
    if (!getAllObjects(templateObject, list)) {
        return false;
    }

    if (!list.size())
        return false;

    for (const auto& obj :qAsConst(list)) {

        if (!changeAction(obj)) {
            return false;
        };

        if (!updateObject(obj)) {
            return false;
        };
    }

    return true;
}

SqlDBCasheWriteMode ISqlDB::getMode() const {
    return _mode;
}

void ISqlDB::setMode(const SqlDBCasheWriteMode &mode) {
    _mode = mode;
}

void ISqlDB::globalUpdateDataBasePrivate(qint64 currentTime) {
    QMutexLocker lock(&_saveLaterMutex);

    for (auto it = _changes.begin(); it != _changes.end(); ++it) {

        if (writer() && writer()->isValid()) {

            auto obj = it.value();

            if (!obj || !obj->isValid()) {
                deleteFromCache(obj);

                QuasarAppUtils::Params::log("writeUpdateItemIntoDB failed when"
                                            " db object is not valid! obj=" +
                                            obj->toString(),
                                            QuasarAppUtils::VerboseLvl::Error);
                continue;
            }

            bool saveResult = false;

            switch (it.key()) {
            case CacheAction::Insert: {
                saveResult = writer()->insertObject(obj, true);
                break;
            }
            case CacheAction::Update: {
                saveResult = writer()->updateObject(obj, true);
                break;
            }
            case CacheAction::Delete: {
                saveResult = writer()->deleteObject(obj, true);
                break;
            }
            default: {
                QuasarAppUtils::Params::log("The Object of the cache have wrong type " +
                                            obj->toString(),
                                            QuasarAppUtils::VerboseLvl::Warning);

                continue;
            }
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

    _changes.clear();
    setLastUpdateTime(currentTime);
}

qint64 ISqlDB::getUpdateInterval() const {
    return updateInterval;
}

void ISqlDB::setUpdateInterval(const qint64 &value) {
    updateInterval = value;
}

}
