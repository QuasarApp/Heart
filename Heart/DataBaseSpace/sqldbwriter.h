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
 *  Befor working with database you need to initialize it. The SqlDBWriter support a any sql databases,
 *  For list of supported drivers see the Qt Docummentation https://doc.qt.io/qt-5/sql-driver.html
 *  For initialize a custom database driver you need to set an own params see the SqlDBWriter::initDb metthod.
 */
class HEARTSHARED_EXPORT SqlDBWriter : public iObjectProvider
{
public:
    SqlDBWriter();

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
     * @param params This is params of initialize a database. For more information about availabe and supported parameters see the description SqlDBWriter::getInitParams method.
     *  For more information see description of the SqlDBWriter::getInitParams method.
     * @return true if database initialize successful.
     */
    virtual bool initDb(const QVariantMap &params);

    /**
     * @brief isValid This method return true if database is successful inited and working correctly.
     * @return true if database working is correctly.
     */
    virtual bool isValid() const;

    bool getAllObjects(const PKG::DBObject &templateObject,  QList<const PKG::DBObject *> &result) override;
    bool saveObject(const QH::PKG::DBObject *ptr) override;
    bool deleteObject(const QH::PKG::DBObject *ptr) override;

    /**
     * @brief databaseLocation This method return location of database.
     * If it is sqllite then return path to db file else return database name.
     * @return path or name of database.
     */
    QString databaseLocation() const;

    virtual ~SqlDBWriter() override;

protected:

    /**
     * @brief enableFK This method enable forign key for the sqlite database
     * @return return true if new settings set is correctly
     */
    bool enableFK();

    /**
     * @brief disableFK This method disable forign key for the sqlite database
     * @return return true if new settings set is correctly
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
     * - DBPass - This is password of a remoute databse.
     * - DBLogin - This is login of a remoute database.
     * - DBHost - This is host addres of a remoute database.
     * - DBPort - port of a remoute database.

     * @param initFile This is path to json file with database configuration.
     * @return The Map with readed configuration. IF file is incorrect or not exists then retun an empty map.
     *
     */
    virtual QVariantMap getInitParams(const QString& initFile) const;

    /**
     * @brief defaultInitPararm This method retrun default pdrameters of the database.
     * @param initFile
     * @return
     *
     *      * Support parameters of database:
     *
     * - DBDriver - This is sql driver of data base for more information see The Qt Documentatuons https://doc.qt.io/qt-5/sql-driver.html
     * - DBFilePath - This is path to file of data base (sqlite only). This is phusical location of sqlite database.
     * - DBInitFile - This is sql file with sql code (structure) with default structure of the database.
     * - DBPass - This is password of a remoute databse.
     * - DBLogin - This is login of a remoute database.
     * - DBHost - This is host addres of a remoute database.
     * - DBPort - port of a remoute database.

     */
    virtual QVariantMap defaultInitPararm() const;

    /**
     * @brief saveQuery This method execute save query of object.
     *  For more Information see DBObject::prepareSaveQuery.
     * @param ptr This is pointer to object that need to save into a database.
     * @return true if function finished seccussful
     */
    virtual bool saveQuery(const QH::PKG::DBObject *ptr) const;

    /**
     * @brief selectQuery generate select query to database from parameters.
     * @param requestObject This is template object for generate select query.
     * @param result This isreturn values
     * @return true if all goodelse false
     */
    virtual bool selectQuery(const QH::PKG::DBObject &requestObject,
                             QList<const QH::PKG::DBObject *> &result);

    /**
     * @brief deleteQuery This method prepare the delete object query.
     * @param deleteObject This is tempalte object for generate a  delete query.
     * @return true if query generated successful.
     */
    virtual bool deleteQuery(const QH::PKG::DBObject *deleteObject) const;

    QSqlDatabase db;

private:

    /**
     * @brief workWithQuery - this base function for all prepareQuery functions.
     * steps work : call prepareFunc, call exec , call cb.
     * @param q - query object with a request
     * @param prepareFunc - function with pripare data for query
     * @param cb - call after success exec and prepare steps
     * @return true if all steps finished successful
     */
    bool workWithQuery(QSqlQuery &q,
                      const std::function< PKG::PrepareResult (QSqlQuery &)> &prepareFunc,
                      const std::function<bool()>& cb) const;

    bool exec(QSqlQuery *sq, const QString &sqlFile);

    bool initSuccessful = false;
    QVariantMap _config;

};

}
#endif // SQLDBWRITER_H
