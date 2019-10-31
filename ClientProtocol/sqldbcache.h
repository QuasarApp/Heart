#ifndef SQLDBCASHE_H
#define SQLDBCASHE_H

#include "iobjectprovider.h"

#include <QMap>
#include <QHash>
#include <QSet>
#include <QVariantMap>
#include <clientprotocol.h>
#include <QMutex>

namespace ClientProtocol {

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
class CLIENTPROTOCOLSHARED_EXPORT SqlDBCache: public QObject, public iObjectProvider
{
    Q_OBJECT


public:
    SqlDBCache(qint64 updateInterval = DEFAULT_UPDATE_INTERVAL);
    ~SqlDBCache() override;

    /**
     * @brief addDbObject
     * @param obj
     * @return
     */
    bool addDbObject(const DBObject* obj);

    /**
     * @brief removeDbObject
     * @param obj
     * @return
     */
    bool removeDbObject(const DBObject* obj);


    /**
     * @brief writer
     * @return weak pointer to writer
     */
    QWeakPointer<SqlDBWriter> writer() const;

    /**
     * @brief setWriter
     * @param writer
     */
    void setWriter(QWeakPointer<SqlDBWriter> writer);

    bool getObject(const QString &table, int id, QWeakPointer<DBObject> *result) override;
    bool saveObject(QWeakPointer<DBObject> saveObject) override;
    bool deleteObject(const QString &table, int id) override;

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

    QSharedPointer<SqlDBWriter> _writer = nullptr;

    QHash<QString, QHash <int, QSharedPointer<DBObject>>>  _cache;
    QHash<QString, QList<int>>  _needToSaveCache;

    virtual void globalUpdateDataBasePrivate(qint64 currentTime);
    virtual void globalUpdateDataBase(SqlDBCasheWriteMode mode = SqlDBCasheWriteMode::Default);

signals:
    void sigItemChanged(int id, QWeakPointer<DBObject> obj);

};

}
#endif // SQLDBCASHE_H
