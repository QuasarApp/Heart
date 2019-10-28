#ifndef SQLDBWRITER_H
#define SQLDBWRITER_H

#include <QObject>
#include <QSqlDatabase>
#include <QDir>
#include <QSqlQuery>
#include "clientprotocol_global.h"
#include "config.h"
#include "dbtablebase.h"
#include "iobjectprovider.h"
#include <QVariant>

class QSqlQuery;
class QSqlDatabase;
class QSqlQuery;
class PlayerDBData;

namespace ClientProtocol {

class DBObject;

/**
 * @brief The SqlDBWriter class
 */
class CLIENTPROTOCOLSHARED_EXPORT SqlDBWriter : public iObjectProvider
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


public:
    SqlDBWriter();

    /**
     * @brief initDb
     * @param path
     * @return
     */
    virtual bool initDb(const QString &initDbParams = DEFAULT_DB_PATH);

    /**
     * @brief isValid
     * @return
     */
    virtual bool isValid() const;

    /**
     * @brief getObject
     * @return
     */
    bool getObject(const QString &table, int id, QWeakPointer<DBObject> *result) override;

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

};

}
#endif // SQLDBWRITER_H
