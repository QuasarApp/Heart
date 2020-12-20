/*
 * Copyright (C) 2018-2020 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
*/

#include "sqldbwriter.h"
#include "sqlitedbcache.h"

namespace QH {

// Private implementation.
class SQLiteDBCachePrivate: public SqlDBWriter {


    // SqlDBWriter interface
protected:
    QSqlDatabase initSqlDataBasse(const QString &driverName,
                                  const QString &name) override;

    bool initDb(const QVariantMap &params) override;

    friend class SQLiteDBCache;
};

QSqlDatabase SQLiteDBCachePrivate::initSqlDataBasse(const QString &driverName, const QString &name) {

    return QSqlDatabase::addDatabase(driverName,
                                     name + "::memory:");

}

bool SQLiteDBCachePrivate::initDb(const QVariantMap &params) {

    if (params.value("DBDriver").toString() != "QSQLITE") {
        QuasarAppUtils::Params::log(" the SQLiteDBCache class support only the QSQLITE driver.",
                                    QuasarAppUtils::Error);
        return false;
    }

    return SqlDBWriter::initDb(params);
}

// Main class members.
SQLiteDBCache::SQLiteDBCache() {
    _private = new SQLiteDBCachePrivate();
}

bool SQLiteDBCache::init(const QVariantMap &params) {

    if (!_private->initDb(params)) {
        return false;
    }

    return ISqlDBCache::init(params);
}

void SQLiteDBCache::deleteFromCache(const QSharedPointer<PKG::DBObject> &delObj) {
    _private->deleteObject(delObj, true);
}

bool SQLiteDBCache::insertToCache(const QSharedPointer<PKG::DBObject> &obj) {
    return _private->insertObject(obj, true);
}

bool SQLiteDBCache::updateCache(const QSharedPointer<PKG::DBObject> &obj) {
    return _private->updateObject(obj, true);
}

QList<QSharedPointer<QH::PKG::DBObject>>&&
SQLiteDBCache::getFromCache(const PKG::DBObject *obj) {

}

void SQLiteDBCache::pushToQueue(const QSharedPointer<PKG::DBObject> &obj,
                                PKG::MemberType type) {

}

void SQLiteDBCache::globalUpdateDataBasePrivate(qint64 currentTime) {

}

}
