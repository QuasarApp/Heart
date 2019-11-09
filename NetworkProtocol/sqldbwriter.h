#ifndef SQLDBWRITER_H
#define SQLDBWRITER_H

#include <QObject>
#include <QSqlDatabase>
#include <QDir>
#include <QSqlQuery>
#include "networkprotocol_global.h"
#include "config.h"
#include "dbtablebase.h"
#include "iobjectprovider.h"
#include <QVariant>

class QSqlQuery;
class QSqlDatabase;
class QSqlQuery;
class PlayerDBData;

namespace NetworkProtocol {

class DBObject;

/**
 * @brief The SqlDBWriter class
 */
class NETWORKPROTOCOLSHARED_EXPORT SqlDBWriter : public iObjectProvider
{
private:

    QString tablesListMySql();
    QString tablesListSqlite();

    QString describeQueryMySql(const QString& tabme);
    QString describeQuerySqlite(const QString& tabme);

    QString getTablesQuery();
    QString describeQuery(const QString& tabme);

    bool exec(QSqlQuery *sq, const QString &sqlFile);

    bool initSuccessful = false;
    QVariantMap _config;

protected:

    /**
     * @brief enableFK - enavle forign ke for sqlite db
     * @return return true if all good
     */
    bool enableFK();

    /**
     * @brief enableFK - disavle forign ke for sqlite db
     * @return return true if all good
     */
    bool disableFK();



    /**
     * @brief getInitPararm
     * @param initFile
     * @return
     *
     * Params :
     * DBDriver - driver of db see https://doc.qt.io/qt-5/sql-driver.html
     * DBFilePath - path to file of data base (sqlite only)
     * DBInitFile - sql file with init state database
     * DBPass - pass of remote db
     * DBLogin - login of remote db
     * DBHost - host addres of reote db
     * DBPort - port of reote db

     */
    virtual QVariantMap getInitParams(const QString& initFile) const;

    /**
     * @brief defaultInitPararm
     * @param initFile
     * @return
     */
    virtual QVariantMap defaultInitPararm() const;

    /**
     * @brief getType
     * @param str
     * @return
     */
    virtual QVariant::Type getType(const QString& str);

    QSqlQuery query;
    QSqlDatabase db;
    QHash<QString, DbTableBase> _dbStruct;

    /**
     * @brief generateHeaderOfQuery - generate list of columns header for update
     * @param retQuery return value
     * @return true if all good
     */
    virtual bool generateHeaderOfQuery(QString& retQuery,
                                       const DbTableBase& tableStruct) const;

    /**
     * @brief generateSourceOfQuery - enerate list of columns header for update
     * @param retQuery return value
     * @param retBindValue list of bind value, after invoce of this method need invoce
     * @return
     */
    virtual bool generateSourceOfQuery(QString& retQuery,
                                       QList<QPair<QString, QVariant> > &retBindValue,
                                       const DbTableBase& tableStruct,
                                       const QVariantMap &map) const;

    /**
     * @brief getBaseQueryString private implementation of getQueryMethods
     * @param queryString
     * @param query
     * @return
     */
    virtual bool getBaseQueryString(QString queryString,
                                    QSqlQuery *query,
                                    const DbTableBase &tableStruct,
                                    const QVariantMap &objMap = {}) const;

    // 0 - table name
    // 1 - headers of update values
    // 2 - update values
    virtual bool saveQuery(const QWeakPointer<DBObject> &ptr) const;

    /**
     * @brief selectQuery generate select query to database from parameters
     * @param returnList - return values
     * @param table - table name of query
     * @param key - compare key (column) for select is default it is id
     * @param val - compare value
     * @return true if all goodelse false
     */
    virtual bool selectQuery(QList<QSharedPointer<DBObject>>& returnList,
                             const QString& table,
                             const QString &key,
                             const QVariant &val);

    virtual bool deleteQuery(const QString &table, int id) const;

    virtual bool checkTableStruct(const QWeakPointer<DBObject> &ptr);


public:
    SqlDBWriter();

    /**
     * @brief initDb
     * @param path
     * @return
     */
    virtual bool initDb(const QString &initDbParams = DEFAULT_DB_PATH);

    /**
     * @brief initDb
     * @param path
     * @return
     */
    virtual bool initDb(const QVariantMap &params);

    /**
     * @brief isValid
     * @return
     */
    virtual bool isValid() const;

    /**
     * @brief getObject
     * @return
     */
    bool getObject(const QString &table, int id, QSharedPointer<DBObject> *result) override;


    /**
     * @brief getObjects
     * @param table
     * @param key - the key by which the value will be searched
     * @param val - value for compare
     * @param result list of db objects (ret value)
     * @return true if all good
     */
    bool getObjects(const QString &table, const QString &key,
                    QVariant val, QList<QSharedPointer<DBObject> > &result) override;

    /**
     * @brief saveObject
     * @return
     */
    bool saveObject(QWeakPointer<DBObject> saveObject) override;

    /**
     * @brief deleteObject
     * @return
     */
    bool deleteObject(const QString &table, int id) override;



    virtual ~SqlDBWriter() override;


    QHash<QString, QSharedPointer<DbTableBase> > getDbStruct() const;
    void setDbStruct(const QHash<QString, QSharedPointer<DbTableBase> > &dbStruct);
};

}
#endif // SQLDBWRITER_H
