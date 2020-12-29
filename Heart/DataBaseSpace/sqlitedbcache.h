/*
 * Copyright (C) 2018-2020 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
*/

#ifndef SQLITEDBCACHE_H
#define SQLITEDBCACHE_H

#include "isqldbcache.h"

namespace QH {

/**
 * SQLiteDBCachePrivate is private implementation of the SQLiteDBCache.
 */
class SQLiteDBCachePrivate;

/**
 * @brief The SQLiteDBCache class using sqliete database for save all temp values.
 * This class use the in memory mode of the sqlite https://sqlite.org/inmemorydb.html
 * This class can be save set objects but available only for the sqlite database.
 * Fo another database type use the SqlDBCache class.
 * @note if you want use the cache then you need to gat only one application with accses to the database
 * else your cache will be have a depricated or invalid data of objects.
 *
 */
class SQLiteDBCache : public ISqlDBCache
{
public:
    SQLiteDBCache();

    // ISqlDBCache interface
    bool init(const QVariantMap &params) override;

protected:
    void deleteFromCache(const QSharedPointer<QH::PKG::DBObject> & delObj) override;
    bool insertToCache(const QSharedPointer<QH::PKG::DBObject> &obj) override;
    bool updateCache(const QSharedPointer<QH::PKG::DBObject> &obj) override;
    QList<QSharedPointer<QH::PKG::DBObject>>&& getFromCache(const QH::PKG::DBObject *obj) override;

private:
    SQLiteDBCachePrivate * _private = nullptr;


};
}
#endif // SQLITEDBCACHE_H
