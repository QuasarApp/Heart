/*
 * Copyright (C) 2018-2020 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
*/

#ifndef SQLDBCASHE_H
#define SQLDBCASHE_H

#include "dbcachekey.h"
#include "iobjectprovider.h"

#include <QMap>
#include <QHash>
#include <QSet>
#include <QVariantMap>
#include <QMutex>
#include "config.h"
#include "basedefines.h"

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
 * @brief The SqlDBCache class it is db cache and bridge for DbWriters.
 * Wor Scheme of the database cache:
 *
 * \image html DatabseCache.svg width=800px
 */
class HEARTSHARED_EXPORT SqlDBCache: public QObject, public iObjectProvider
{
    Q_OBJECT

public:
    SqlDBCache(qint64 updateInterval = DEFAULT_UPDATE_INTERVAL);
    ~SqlDBCache() override;

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

    bool getAllObjects(const PKG::DBObject &templateObject,  QList<const PKG::DBObject *> &result) override;

    bool saveObject(const PKG::DBObject* saveObject) override;
    bool deleteObject(const PKG::DBObject* delObj) override;

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

    /**
     * @brief deleteFromCache This mrthod delete object from db cache, bat not from database.
     * @param delObj [pointer to object for delete.
     * @return true if object has been delete successful.
     */
    virtual void deleteFromCache(const PKG::DBObject *delObj);

    /**
     * @brief saveToCache This method save object into cache,  but not database.
     * @param obj This is object for save into cache.
     * @return true if save finished successful.
     */
    virtual bool saveToCache(const PKG::DBObject *obj);

    /**
     * @brief getFromCache This method get database objcet from cache.
     * @param objKey This is database cache id.
     * @return database Object from cache. if object with objKey not exits return nullptr.
     */
    virtual PKG::DBObject* getFromCache(uint objKey);

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
    QMutex _saveLaterMutex;
    QMutex _cacheMutex;

    SqlDBCasheWriteMode _mode;

    QHash<uint, PKG::DBObject*>  _cache;
    QSet<uint>  _needToSaveCache;
    SqlDBWriter* _writer = nullptr;



signals:
    void sigItemChanged(const PKG::DBObject *obj);
};

}
#endif // SQLDBCASHE_H
