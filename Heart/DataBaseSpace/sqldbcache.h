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
 * @brief The SqlDBCache class is default universal implementation of the database cache.
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
    void deleteFromCache(const PKG::DBObject *delObj) override;
    bool insertToCache(const PKG::DBObject *obj) override;
    bool updateCache(const PKG::DBObject *obj) override;
    PKG::DBObject *getFromCache(const PKG::DBObject *obj) override;
    void globalUpdateDataBasePrivate(qint64 currentTime) override;

private:
    QMutex _saveLaterMutex;
    QMutex _cacheMutex;
    QHash<uint, PKG::DBObject*>  _cache;
    QHash<PKG::MemberType, const PKG::DBObject*>  _needToSaveCache;

};

}
#endif // SQLDBCASHE_H
