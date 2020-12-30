/*
 * Copyright (C) 2018-2020 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
*/

#ifndef ISQLDBCACHE_H
#define ISQLDBCACHE_H

#include "dbcachekey.h"
#include "iobjectprovider.h"

#include <QMap>
#include <QHash>
#include <QSet>
#include <QVariantMap>
#include <QMutex>
#include "config.h"
#include "basedefines.h"
#include "softdelete.h"

namespace QH {

class SqlDBWriter;
class DbAddress;

/**
 * @brief The SqlDBCasheWriteMode enum contains list of available modes of write data ito database.
 */
enum class SqlDBCasheWriteMode: int {
    /// This is default mode. The database is written to the database with each function call no more often than the specified update interval. The recording is done in thread in which this function was called.
    Default = 0x0,
    /// This mode write a changes into another thread.
    On_New_Thread = 0x1,
    /// This mode writes all changes to the database as soon as they come to the cache.
    Force = 0x2,
} ;

/**
 * @brief The CacheAction enum contains types of database cache actions.
 * The any database caches save all changes in to hardware database.
 *  For saving all changes it use hash map with objects and its actions.
 * Every type invokes own method for running an action of object.
 */
enum class CacheAction: int {
    /// Do nothing.
    None,
    /// Invoke the SqlDBWriter::insertObject method of a private database writer implementation.
    Insert,
    /// Invoke the SqlDBWriter::updateObject method of a private database writer implementation.
    Update,
    /// Invoke the SqlDBWriter::deleteObject method of a private database writer implementation.
    Delete
};

/**
 * @brief The ISqlDBCache class it is db cache and bridge for DbWriters.
 * Wor Scheme of the database cache:
 *
 * \image html DatabseCache.svg width=800px
 *
 * This class implements only base functions of the cache. You need to override this class and create implementation for all methods of work with tehe cacha.
 * * updateCache
 * * insertToCache
 * * deleteFromCache
 * * getFromCache
 *
 * @note Objects of all implementation of this classs must be deleted using the softDelete.
 * The softDelete method create save all cached data into database.
 * If you try delete this object wthout using softDelete method then distructor of this object emit runtime error exeption (on the debug mode only).
 *
 */
class HEARTSHARED_EXPORT ISqlDBCache: public QObject, public iObjectProvider, public SoftDelete
{
    Q_OBJECT

public:
    /**
     * @brief SqlDBCache This is default constructor of dbcache
     * @param updateInterval See the SqlDBCache::setUpdateInterval for more information.
     * @param mode See the SqlDBCache::setMode method  for more information.
     */
    ISqlDBCache(qint64 updateInterval = DEFAULT_UPDATE_INTERVAL,
               SqlDBCasheWriteMode mode = SqlDBCasheWriteMode::Default);
    ~ISqlDBCache() override;

    /**
     * @brief writer This method return is database writer object. For more inforamtion about writer see the SqlDBWriter class.
     * @return the pointer to writer.
     */
    SqlDBWriter* writer() const;

    /**
     * @brief setWriter This methd set new writer for this cache.
     * @param writer new writer.
     */
    void setWriter(SqlDBWriter* writer);

    bool getAllObjects(const PKG::DBObject &templateObject,
                       QList<QSharedPointer<QH::PKG::DBObject>> &result) override;

    bool updateObject(const QSharedPointer<QH::PKG::DBObject>& saveObject,
                      bool wait = false) override;
    bool deleteObject(const QSharedPointer<QH::PKG::DBObject>& delObj,
                      bool wait = false) override;
    bool insertObject(const QSharedPointer<QH::PKG::DBObject>& saveObject,
                      bool wait = false) override;

    /**
     * @brief changeObjects This method change objecst of the database.
     * @param templateObject This is template for get objects from database.
     * @param changeAction This is lymbda function for work with non const pointer of the object for change.
     * @note If templateObject selecte more than 1 objects then the lyambda function call for each all selected objects.
     * @return true if function finished succesful.
     */
    bool changeObjects(const PKG::DBObject &templateObject,
                       const std::function<bool (const QSharedPointer<PKG::DBObject> &)> &changeAction);
    /**
     * @brief getUpdateInterval This method return update interval for save changes into database. This is work for default and On_New_Thread mdes. For more information see the QH::SqlDBCasheWriteMode enum.
     * @return time in msecs
     */
    qint64 getUpdateInterval() const;

    /**
     * @brief getUpdateInterval This method set new value of an update interval for save changes into database. This is work for default and On_New_Thread mdes. For more information see the QH::SqlDBCasheWriteMode enum.
     * @param value this is new update interval.
     */
    void setUpdateInterval(const qint64 &value);

    /**
     * @brief init This method init the cache object and invoke the  SqlDBWriter::initDb method.
     * @param initDbParams This is path to json file with database params.
     * @return true if initialize of database finished successful.
     */
    virtual bool init(const QString &initDbParams = "");

    /**
     * @brief init This method init the cache object and invoke the  SqlDBWriter::initDb method.
     * @param params This is database params map.
     * @return true if initialize of database finished successful.
     */
    virtual bool init(const QVariantMap &params);

    void setSQLSources(const QStringList &list) override;

protected:
    virtual void prepareForDelete() override;

    /**
     * @brief getLastUpdateTime This method return time of last update.
     * @return msec of the last update.
     */
    qint64 getLastUpdateTime() const;

    /**
     * @brief setLastUpdateTime This method set new value of the update time.
     * @param value new time of update.
     */
    void setLastUpdateTime(const qint64 &value);

    /**
     * @brief deleteFromCache This method delete object from db cache, bat not from database.
     * @param delObj [pointer to object for delete.
     * @return true if object has been delete successful.
     */
    virtual void deleteFromCache(const QSharedPointer<QH::PKG::DBObject> &delObj) = 0;

    /**
     * @brief insertToCache This method insert object into cache,  but not database.
     *  If Object exists in the cache this method return false.
     * @param obj This is object for save into cache.
     * @return true if insert finished successful. If Object exists then return false.
     */
    virtual bool insertToCache(const QSharedPointer<QH::PKG::DBObject> &obj)  = 0;

    /**
     * @brief updateCache This method update alredy exits object on the cache, but not database.
     * @param obj This is object with chages for updae the object rtom cache.
     * @return true if save finished successful. If the obj mot exits in the cache then return false.
     */
    virtual bool updateCache(const QSharedPointer<QH::PKG::DBObject> &obj) = 0;

    /**
     * @brief getFromCache This method return strong pointer to the database objcet from cache (pool).
     * @param objKey This is database cache id.
     * @return database Object from cache. if object with objKey not exits return nullptr.
     */
    virtual QList<QSharedPointer<QH::PKG::DBObject>>
    getFromCache(const PKG::DBObject *obj) = 0;

    /**
     * @brief pushToQueue this method should be add the object to the update queue in the physical data dash.
     * @param obj This is obje for update.
     * @param type This is type of action. For more information see the CacheAction enum.
     */
    virtual void pushToQueue(const QSharedPointer<QH::PKG::DBObject> &obj,
                             CacheAction type);

    /**
     * @brief getMode This method return mode of work databnase cache. For mmore information see the QH::SqlDBCasheWriteMode enum.
     * @return QH::SqlDBCasheWriteMode value.
     */
    SqlDBCasheWriteMode getMode() const;

    /**
     * @brief setMode This method set a new value of the mode database cache.
     * @param mode This is new value of mode working the database cahce
     */
    void setMode(const SqlDBCasheWriteMode &mode);

    /**
     * @brief globalUpdateDataBasePrivate This merhod update(write) all data from cache into database.
     * Override this methd if you want change method of writinga data from cache.
     * @param currentTime This is current time for saving time of the invoke of this method.
     */
    virtual void globalUpdateDataBasePrivate(qint64 currentTime);

    /**
     * @brief globalUpdateDataBase This is base method for syncing data from the cache with database.
     * @param mode This is mode writing into database.
     */
    virtual void globalUpdateDataBase(SqlDBCasheWriteMode mode = SqlDBCasheWriteMode::Default);

private:
    qint64 lastUpdateTime = 0;
    qint64 updateInterval = DEFAULT_UPDATE_INTERVAL;

    SqlDBCasheWriteMode _mode;

    SqlDBWriter* _writer = nullptr;

    QHash<CacheAction, QSharedPointer<QH::PKG::DBObject>> _changes;
    QMutex _saveLaterMutex;

signals:
    void sigItemChanged(const PKG::DBObject *obj);

};

/**
 * @brief qHash calc hash of the CacheAction enum.
 * @param action input data
 * @return hash value of the action.
 */
constexpr inline uint qHash(CacheAction action) {
    return static_cast<uint>(action);
}

}

#endif // ISQLDBCACHE_H
