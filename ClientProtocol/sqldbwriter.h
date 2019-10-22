#ifndef SQLDBWRITER_H
#define SQLDBWRITER_H

#include <QObject>
#include <QSqlDatabase>
#include <QDir>
#include <QSqlQuery>
#include "clientprotocol_global.h"
#include "config.h"

class QSqlQuery;
class QSqlDatabase;
class QSqlQuery;
class PlayerDBData;

namespace ClientProtocol {

/**
 * @brief The SqlDBWriter class
 */
class CLIENTPROTOCOLSHARED_EXPORT SqlDBWriter
{
private:
    bool exec(QSqlQuery *sq, const QString &sqlFile);

    bool initSuccessful = false;

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

    QSqlQuery query;
    QSqlDatabase db;


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

    virtual ~SqlDBWriter();

};

}
#endif // SQLDBWRITER_H
