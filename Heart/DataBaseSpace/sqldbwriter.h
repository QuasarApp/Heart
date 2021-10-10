/*
 * Copyright (C) 2018-2021 QuasarApp.
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
#include "async.h"
#include "heart_global.h"
#include "config.h"
#include "iobjectprovider.h"
#include <QVariant>
#include <QCoreApplication>
#include <dbobject.h>

class QSqlDatabase;
class PlayerDBData;

namespace QH {

/**
 * @brief The SqlDBWriter class. This class write and read objects from database (hard level).
 *  Before working with database you need to initialize it. The SqlDBWriter support a any sql databases,
 *  For list of supported drivers see the Qt Docummentation https://doc.qt.io/qt-5/sql-driver.html
 *  For initialize a custom database driver you need to set an own params see the SqlDBWriter::initDb metthod.
 *  This class work only on own thread. For change working thread use the Qt method moveToThread.
 */
class HEARTSHARED_EXPORT SqlDBWriter : public Async, public iObjectProvider
{
    Q_OBJECT
public:
    SqlDBWriter(QThread *thread, QObject* ptr = nullptr);

    /**
     * @brief initDb This method is wraper of the initDb(const QVariantMap &params) method.
     *  This implementation read a initDbParams file and get all params from json.
     * @note this method must be invoked in the thread as a working thread of this object.
     * @param initDbParams This is path to  json configuration file with settings of database.
     *  For more information see description of the SqlDBWriter::getInitParams method.
     * @return true if database initialize successful.
     */
    virtual bool initDb(const QString &initDbParams = DEFAULT_DB_PATH);

    /**
     * @brief initDb This method initialize a database params and create a new database if the old database not exists.
     * @note this method must be invoked in the some thread as a working thread of this object.
     * @param params This is params of initialize a database. For more information about available and supported parameters see the description SqlDBWriter::getInitParams method.
     *  For more information see description of the SqlDBWriter::getInitParams method.
     * @return true if database initialize successful.
     */
    virtual bool initDb(const QVariantMap &params);

    /**
     * @brief isValid This method return true if database is successful inited and working correctly.
     * @return true if database working is correctly.
     */
    virtual bool isValid() const;

    bool getAllObjects(const PKG::DBObject &templateObject,
                       QList<QSharedPointer<PKG::DBObject>> &result) override;
    bool updateObject(const QSharedPointer<PKG::DBObject> &ptr, bool wait = false) override;
    bool deleteObject(const QSharedPointer<PKG::DBObject> &ptr, bool wait = false) override;
    bool insertObject(const QSharedPointer<PKG::DBObject> &ptr, bool wait = false) override;
    void setSQLSources(const QStringList &list) override;
    bool doQuery(const QString& query, bool wait = false, QSqlQuery* result = nullptr) const override;

    /**
     * @brief databaseLocation This method return location of database.
     * If it is sqlite then return path to db file else return database name.
     * @return path or name of database.
     */
    QString databaseLocation() const;

    virtual ~SqlDBWriter() override;

    /**
     * @brief updateQuery This method execute update query of object.
     *  For more Information see DBObject::prepareUpdateQuery.
     * @param ptr This is strong pointer to object that need to update into a database.
     * @return true if function finished successful.
     */
    virtual bool updateQuery(const QSharedPointer<QH::PKG::DBObject> &ptr) const;

    /**
     * @brief selectQuery generate select query to database from parameters.
     * @param requestObject This is template object for generate select query.
     * @param result This is return values.
     * @return true if all good else false.
     */
    virtual bool selectQuery(const QH::PKG::DBObject &requestObject,
                             QList<QSharedPointer<QH::PKG::DBObject>> &result);

    /**
     * @brief deleteQuery This method prepare the delete object query.
     * @param deleteObject This is tempalte object for generate a  delete query.
     * @return true if query generated successful.
     */
    virtual bool deleteQuery(const QSharedPointer<QH::PKG::DBObject> &deleteObject) const;

    /**
     * @brief insertQuery This method prepare the insert object query.
     * @param insertObject This is strong pointer of object for generate the insert query.
     * @return true if query generated successful.
     */
    virtual bool insertQuery(const QSharedPointer<QH::PKG::DBObject>& insertObject) const;


protected slots:


protected:

    /**
     * @brief enableFK This method enable foreign key for the sqlite database.
     * @return return true if new settings set is correctly.
     */
    bool enableFK();

    /**
     * @brief disableFK This method disable foreign key for the sqlite database.
     * @return return true if new settings set is correctly.
     */
    bool disableFK();

    /**
     * @brief getInitPararm This method read parameters of database.
     *
     * Support parameters of database:
     *
     * - DBDriver - This is sql driver of data base for more information see The Qt Documentatuons https://doc.qt.io/qt-5/sql-driver.html
     * - DBFilePath - This is path to file of data base (sqlite only). This is phusical location of sqlite database.
     * - DBInitFile - This is sql file with sql code (structure) with default structure of the database.
     * - DBPass - This is password of a remote database.
     * - DBLogin - This is login of a remote database.
     * - DBHost - This is host addres of a remote database.
     * - DBPort - port of a remote database.

     * @param initFile This is path to json file with database configuration.
     * @return The Map with readed configuration. IF file is incorrect or not exists then return an empty map.
     *
     */
    virtual QVariantMap getInitParams(const QString& initFile) const;

    /**
     * @brief defaultInitPararm This method return default parameters of the database.
     * @return map of database params.
     *
     * Support parameters of database:
     *
     * - DBDriver - This is sql driver of data base for more information see The Qt Documentations https://doc.qt.io/qt-5/sql-driver.html
     * - DBFilePath - This is path to file of data base (sqlite only). This is phusical location of sqlite database.
     * - DBInitFile - This is sql file with sql code (structure) with default structure of the database.
     * - DBPass - This is password of a remote database.
     * - DBLogin - This is login of a remote database.
     * - DBHost - This is host address of a remote database.
     * - DBPort - port of a remote database.

     */
    virtual QVariantMap defaultInitPararm() const;

    /**
     * @brief initSqlDataBasse This method create ad database connection.
     * Without configuration. This method invoked in the initDbPrivate method.
     * @note Overrde this method for create f custom method of initialization of the db connection.
     * @param driverName This is name of sql driver for more information see about Qt Sql Drivers.
     * @param name This is name of path of database.
     * @return database object.
     */
    virtual QSqlDatabase initSqlDataBasse(const QString &driverName,
                                          const QString &name);

    /**
     * @brief db This method return db connection. If SqlDBWriter in not inited then return nullptr.
     *  For more information about initialization the SqlDBWriter see the SqlDBWriter::initDb method.
     * @return pointer to actual database connection.
     */
    QSqlDatabase* db();

    /**
     * @brief db This is constant implementation of the SqlDBWriter::db method.
     * @return return const pointer to database connection.
     */
    const QSqlDatabase* db() const;

private:

    /**
     * @brief workWithQuery - this base function for all prepareQuery functions.
     * steps work : call prepareFunc, call exec , call cb.
     * @param q - query object with a request.
     * @param prepareFunc - function with prepare data for query.
     * @param cb - call after success exec and prepare steps.
     * @param printErrors This propertye enabled or disabled printing error messages. By default is enabled.
     * @return true if all steps finished successful.
     */
    bool workWithQuery(QSqlQuery &q,
                      const std::function< PKG::PrepareResult (QSqlQuery &)> &prepareFunc,
                      const std::function<bool()>& cb,
                      bool printErrors = true) const;

    bool exec(QSqlQuery *sq, const QString &sqlFile);

    /**
     * @brief initDbPrivate This is private method of initialize database.
     * @param params This is parameters of database.
     * @return true if all good.
     */
    bool initDbPrivate(const QVariantMap &params);

    /**
     * @brief doQueryPrivate This method execute a @a query in this database.
     * @param query This is query that will be executed.
     * @param result This is pointer to result value.
     * @return true if query finished successfull
     */
    bool doQueryPrivate(const QString& query, QSqlQuery* result) const;

    bool initSuccessful = false;
    QVariantMap _config;
    QStringList _SQLSources;

    QSqlDatabase *_db = nullptr;

};

}
#endif // SQLDBWRITER_H
