#include "sqldbcache.h"
#include "quasarapp.h"
#include "sqldbwriter.h"

#include <networkprotocol.h>
#include <dbobject.h>

#include <QDateTime>
#include <QtConcurrent/QtConcurrent>


namespace NetworkProtocol {

void SqlDBCache::globalUpdateDataBasePrivate(qint64 currentTime) {

    QMutexLocker lock(&_saveLaterMutex);

    for (auto listIt = _needToSaveCache.begin(); listIt != _needToSaveCache.end(); ++listIt ) {

        auto list = listIt.value();
        for (int id: list) {

            auto saveObject = _cache.value(listIt.key()).value(id);

            if (!saveObject.isNull() && !_writer.isNull() && _writer->isValid()) {

                if (!saveObject->isValid()) {
                    deleteFromCache(listIt.key(), id);

                    QuasarAppUtils::Params::verboseLog("writeUpdateItemIntoDB failed when"
                                                       " db object is not valid! id=" + QString::number(id),
                                                       QuasarAppUtils::VerboseLvl::Error);
                    continue;
                }

                 if (!_writer->saveObject(saveObject)) {
                     QuasarAppUtils::Params::verboseLog("writeUpdateItemIntoDB failed when"
                                                        " work globalUpdateDataRelease!!! id=" +
                                                         QString::number(id),
                                                        QuasarAppUtils::VerboseLvl::Error);
                 }

                 QuasarAppUtils::Params::verboseLog("writeUpdateItemIntoDB failed when"
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

QWeakPointer<SqlDBWriter> SqlDBCache::writer() const {
    return _writer;
}

void SqlDBCache::setWriter(QWeakPointer<SqlDBWriter> writer) {
    _writer = writer;
}

bool SqlDBCache::getObject(QWeakPointer<DBObject> &obj) {

    auto ref = obj.toStrongRef();

    if (ref.isNull())
        return false;

    int id = ref->getId();
    auto table = ref->tableName();

    auto& tableObj = _cache[table];


    if (!tableObj.contains(id) && !_writer.isNull() && _writer->isValid()) {
        if (!_writer->getObject(obj)) {
            return false;
        }
        _cache[table][id] = ref;
        return true;
    }

    auto &sptr = tableObj[id];

    if (!sptr->isValid()) {
        deleteFromCache(table, id);
        return false;
    }

    obj = sptr;
    return true;
}

bool SqlDBCache::saveObject(const QWeakPointer<DBObject>& saveObject) {

    auto ptr = saveObject.toStrongRef();

    if (!(ptr.isNull() && ptr->isValid())) {
        return false;
    }

    _cache[ptr->tableName()][ptr->getId()] = ptr;

    if (getMode() == SqlDBCasheWriteMode::Force) {
        if (!_writer.isNull() && _writer->isValid()) {
            return _writer->saveObject(saveObject);
        }
    } else {
        _needToSaveCache[ptr->tableName()].push_back(ptr->getId());
        globalUpdateDataBase(_mode);
    }

    return true;


}

bool SqlDBCache::deleteObject(const QWeakPointer<DBObject> &delObj) {
    auto ref =  delObj.toStrongRef();

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
