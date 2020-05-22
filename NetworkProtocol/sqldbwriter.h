/*
 * Copyright (C) 2018-2020 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
*/

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
#include <QCoreApplication>

class QSqlQuery;
class QSqlDatabase;
class QSqlQuery;
class PlayerDBData;

namespace NP {

class DBObject;
class AbstractData;
/**
 * @brief The SqlDBWriter class
 */
class NETWORKPROTOCOLSHARED_EXPORT SqlDBWriter : public iObjectProvider
{
private:

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

    QSqlDatabase db;
    QHash<QString, DbTableBase> _dbStruct;

    // 0 - table name
    // 1 - headers of update values
    // 2 - update values
    virtual bool saveQuery(const WP<AbstractData> &ptr) const;

    /**
     * @brief selectQuery generate select query to database from parameters
     * @param returnList - return values
     * @param table - table name of query
     * @param key - compare key (column) for select is default it is id
     * @param val - compare value
     * @return true if all goodelse false
     */
    virtual bool selectQuery(const SP<DBObject> &obj);

    virtual bool deleteQuery(const WP<AbstractData> &deleteObject) const;

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
    bool getObject(SP<DBObject> &obj) override;

    /**
     * @brief saveObject
     * @return
     */
    bool saveObject(const WP<AbstractData> &saveObject) override;

    /**
     * @brief deleteObject
     * @return
     */
    bool deleteObject(const WP<AbstractData> &deleteObject) override;

    virtual ~SqlDBWriter() override;


    QHash<QString, SP<DbTableBase> > getDbStruct() const;
    void setDbStruct(const QHash<QString, SP<DbTableBase> > &dbStruct);
};

}
#endif // SQLDBWRITER_H
