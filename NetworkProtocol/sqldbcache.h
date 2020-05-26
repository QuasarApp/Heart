/*
 * Copyright (C) 2018-2020 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
*/

#ifndef SQLDBCASHE_H
#define SQLDBCASHE_H

#include "iobjectprovider.h"

#include <QMap>
#include <QHash>
#include <QSet>
#include <QVariantMap>
#include <networkprotocol.h>
#include <QMutex>

namespace NP {

class SqlDBWriter;
class DBObject;

enum class SqlDBCasheWriteMode: int {
    Default = 0x0,
    On_New_Thread = 0x1,
    Force = 0x2,
} ;

/**
 * @brief The SqlDBCache class it is db cache and bridge for DbWriters
 */
class NETWORKPROTOCOLSHARED_EXPORT SqlDBCache: public QObject, public iObjectProvider
{
    Q_OBJECT


public:
    SqlDBCache(qint64 updateInterval = DEFAULT_UPDATE_INTERVAL);
    ~SqlDBCache() override;

    /**
     * @brief writer
     * @return weak pointer to writer
     */
    WP<SqlDBWriter> writer() const;

    /**
     * @brief setWriter
     * @param writer
     */
    void setWriter(const WP<SqlDBWriter> &writer);

    bool getObject(SP<DBObject> &obj) override;
    DBObject* getObjectFromCache(const QString& table, int id);

    bool saveObject(const WP<AbstractData> &saveObject) override;
    bool deleteObject(const WP<AbstractData>& delObj) override;

    /**
     * @brief getUpdateInterval of SqlDBCasheWriteMode::Default mode
     * @return time in msecs
     */
    qint64 getUpdateInterval() const;

    /**
     * @brief setUpdateInterval set update time of SqlDBCasheWriteMode::Default mode
     * @param value time in msecs
     */
    void setUpdateInterval(const qint64 &value);

    /**
     * @brief init
     * @return
     */\
    virtual bool init(const QString &initDbParams = "");

    /**
     * @brief init
     * @return
     */\
    virtual bool init(const QVariantMap &params);

protected:

    /**
     * @brief deleteFromCache - delete object  from db cache, bat not from database.
     * @param table - table of object
     * @param id in table of object
     * @return true if all good
     */
    virtual void deleteFromCache(const QString &table, int id);

    /**
     * @brief saveToCache
     * @param obj
     */
    virtual void saveToCache(const WP<AbstractData> &obj);


    /**
     * @brief getMode
     * @return
     */
    SqlDBCasheWriteMode getMode() const;

    /**
     * @brief setMode
     * @param mode
     */
    void setMode(const SqlDBCasheWriteMode &mode);

    QMutex _saveLaterMutex;

private:
    qint64 lastUpdateTime = 0;
    qint64 updateInterval = DEFAULT_UPDATE_INTERVAL;

    SqlDBCasheWriteMode _mode;

    SP<SqlDBWriter> _writer = nullptr;

    QHash<QString, QHash <int, SP<DBObject>>>  _cache;
    QHash<QString, QList<int>>  _needToSaveCache;

    virtual void globalUpdateDataBasePrivate(qint64 currentTime);
    virtual void globalUpdateDataBase(SqlDBCasheWriteMode mode = SqlDBCasheWriteMode::Default);

signals:
    void sigItemChanged(const WP<AbstractData> &obj);

};

}
#endif // SQLDBCASHE_H
