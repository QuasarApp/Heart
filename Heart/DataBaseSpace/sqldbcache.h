/*
 * Copyright (C) 2018-2020 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
*/

#ifndef SQLDBCASHE_H
#define SQLDBCASHE_H

#include "iobjectprovider.h"
#include "isqldbcache.h"


namespace QH {

/**
 * @brief The SqlDBCache class is universal implementation of the database cache.
 * This implementation have a one hashMap for the databae object.
 *  This is work for all Dabatases types but some objects like a DBObjectSet or DBObjectsRequest class.
 */
class HEARTSHARED_EXPORT SqlDBCache: public ISqlDBCache
{
    Q_OBJECT

public:
    SqlDBCache(qint64 updateInterval = DEFAULT_UPDATE_INTERVAL,
               SqlDBCasheWriteMode mode = SqlDBCasheWriteMode::Default);

    ~SqlDBCache();
    // ISqlDBCache interface
protected:
    void deleteFromCache(const QSharedPointer<QH::PKG::DBObject>& delObj) override;
    bool insertToCache(const QSharedPointer<QH::PKG::DBObject>& obj) override;
    bool updateCache(const QSharedPointer<QH::PKG::DBObject>& obj) override;
    QList<QSharedPointer<QH::PKG::DBObject>>
    getFromCache(const PKG::DBObject *obj) override;

private:
    QSharedPointer<QH::PKG::DBObject> getFromCacheById(quint32 dbKey);


    QMutex _saveLaterMutex;
    QMutex _cacheMutex;
    QHash<uint, QSharedPointer<PKG::DBObject>> _cache;
    QHash<PKG::MemberType, QSharedPointer<QH::PKG::DBObject>>  _changes;
    };

}
#endif // SQLDBCASHE_H
