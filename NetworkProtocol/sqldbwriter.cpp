#include "dbobjectsfactory.h"
#include "dbtablebase.h"
#include "sqldbwriter.h"

#include <QRegularExpression>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QDebug>
#include <QSqlError>
#include <quasarapp.h>
#include <networkprotocol.h>
#include <QJsonDocument>
#include <QJsonObject>
#include <QHash>
#include <dbobject.h>
#include <QSqlRecord>

namespace NetworkProtocol {


QString SqlDBWriter::tablesListMySql() {
    return "show tables";
}

QString SqlDBWriter::tablesListSqlite() {
    return "SELECT name FROM sqlite_master WHERE type ='table' AND name NOT LIKE 'sqlite_%'";
}

QString SqlDBWriter::describeQueryMySql(const QString &tabme) {
    return QString("describe %0").arg(tabme);
}

QString SqlDBWriter::describeQuerySqlite(const QString &tabme) {
    return  QString("pragma table_info('%0')").arg(tabme);
}

QString SqlDBWriter::getTablesQuery() {
    auto driver = _config["DBDriver"].toString();

    if (driver.contains("QSQLITE")) {
        return tablesListSqlite();
    } else if (driver.contains("QMYSQL")) {
        return tablesListMySql();
    }

    return "";
}

QString SqlDBWriter::describeQuery(const QString &tabme) {
    auto driver = _config["DBDriver"].toString();

    if (driver.contains("QSQLITE")) {
        return describeQuerySqlite(tabme);
    } else if (driver.contains("QMYSQL")) {
        return describeQueryMySql(tabme);
    }

    return "";
}

bool SqlDBWriter::exec(QSqlQuery *sq,const QString& sqlFile) {
    QFile f(sqlFile);
    bool result = true;
    if (f.open(QIODevice::ReadOnly)) {

        QString temp, delimiter = ";";
        QTextStream stream(&f);
        stream.setCodec("UTF8");

        while(!stream.atEnd()) {

            temp += stream.readLine();

            if (temp.lastIndexOf("delimiter", -1, Qt::CaseInsensitive) > -1) {

                temp.remove("delimiter", Qt::CaseInsensitive);

                int last = temp.indexOf(QRegularExpression("[^ \f\n\r\t\v]")) + 1;

                int begin = temp.lastIndexOf(QRegularExpression("[^ \f\n\r\t\v]"));

                delimiter = temp.mid(begin, last - begin);

                temp = "";
            } else {
                if (temp.lastIndexOf(delimiter) >- 1) {
                    temp.remove(delimiter);

                    result = result && sq->exec(temp);

                    if (!result) {
                        QuasarAppUtils::Params::verboseLog("exec database error: " +sq->lastError().text(),
                                                           QuasarAppUtils::Error);
                        f.close();
                        return false;
                    }

                    temp = "";
                }
            }
        }

        f.close();

        if (result && !(sq->prepare(getTablesQuery()) && sq->exec())) {
            QuasarAppUtils::Params::verboseLog("init database error: " +sq->lastError().text(),
                                               QuasarAppUtils::Error);
        }

        QStringList tables;
        while(sq->next()) {
            tables.push_back(sq->value(0).toString());
        }

        for(auto &table : tables) {
            if (!(sq->prepare(describeQuery(table)) && sq->exec())) {
                QuasarAppUtils::Params::verboseLog("init database error: " +sq->lastError().text(),
                                                   QuasarAppUtils::Error);
                continue;
            }

            DbTableBase tableInfo = {table, {}};
            while (sq->next()) {
                tableInfo.keys[sq->value(0).toString()] = getType(sq->value(1).toString());
            }

            _dbStruct[table] = tableInfo;
        }
        return result;
    }
    return false;
}

bool SqlDBWriter::enableFK() {

    QString request = QString("PRAGMA foreign_keys = ON");
    if (!query.exec(request)) {
        QuasarAppUtils::Params::verboseLog("request error : " + query.lastError().text());
        return false;
    }

    return true;
}

bool SqlDBWriter::disableFK() {

    QString request = QString("PRAGMA foreign_keys = OFF");
    if (!query.exec(request)) {
        QuasarAppUtils::Params::verboseLog("request error : " + query.lastError().text());
        return false;
    }

    return true;

}

QVariantMap SqlDBWriter::getInitParams(const QString &initFile) const {
    QFile file(initFile);
    QVariantMap res;

    if (file.open(QIODevice::ReadOnly)) {
        QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
        file.close();

        if (!doc.isObject()) {
            return res;
        }

        QJsonObject obj = doc.object();
        return obj.toVariantMap();
    }

    return res;
}

/*
 *  about driver see https://doc.qt.io/qt-5/sql-driver.html
 */
QVariantMap SqlDBWriter::defaultInitPararm() const {
    QVariantMap params;
    params["DBDriver"] = "QSQLITE";
    params["DBFile"] = DEFAULT_DB_PATH;

    return params;
}

SqlDBWriter::SqlDBWriter() {
}

bool SqlDBWriter::initDb(const QString &initDbParams) {
    QVariantMap params;

    if (initDbParams.isEmpty()) {
        params = defaultInitPararm();
    } else {
        params = getInitParams(initDbParams);
    }

    return initDb(params);
}

bool SqlDBWriter::initDb(const QVariantMap &params) {


    _config = params;

    db = QSqlDatabase::addDatabase(params["DBDriver"].toString(),
            QFileInfo(params["DBFilePath"].toString()).fileName());

    if (params.contains("DBFilePath")) {

        auto path = QFileInfo(params["DBFilePath"].toString());
        if (!QDir("").mkpath(path.absolutePath())) {
            return false;
        }

        db.setDatabaseName(path.absoluteFilePath());
    }

    if (params.contains("DBLogin")) {
        db.setPassword(params["DBLogin"].toString());
    }

    if (params.contains("DBPass")) {
        db.setPassword(params["DBPass"].toString());
    }

    if (params.contains("DBHost")) {
        db.setHostName(params["DBHost"].toString());
    }

    if (params.contains("DBPort")) {
        db.setPort(params["DBPort"].toInt());
    }

    query = QSqlQuery(db);

    if (!db.open()) {
        QuasarAppUtils::Params::verboseLog(db.lastError().text(),
                                           QuasarAppUtils::Error);
        return false;
    }

    if (params.contains("DBInitFile")) {
        auto path = QFileInfo(params["DBInitFile"].toString()).absoluteFilePath();

        if (!exec(&query, path)) {
            return false;
        }
    }

    initSuccessful = db.isValid();
    return initSuccessful;
}

bool SqlDBWriter::isValid() const {
    return db.isValid() && db.isOpen() && initSuccessful;
}

bool SqlDBWriter::getObject(const QString& table, int id, QSharedPointer<DBObject> *result) {

    QList<QSharedPointer<DBObject>> res;
    if(!getObjects(table, "id", id, res) && !res.size()) {
        return false;
    }

    *result = res.first().toWeakRef();

    return true;

}

bool SqlDBWriter::getObjects(const QString &table, const QString &key,
                             QVariant val, QList<QSharedPointer<DBObject> > &result) {

    if (!_dbStruct.contains(table)) {
        return false;
    }

    return selectQuery(result, table, key, val);
}


bool SqlDBWriter::saveObject(QWeakPointer<DBObject> saveObject) {
    return saveQuery(saveObject);
}

bool SqlDBWriter::deleteObject(const QString& table, int id) {
    return deleteQuery(table, id);
}

SqlDBWriter::~SqlDBWriter() {
}


#define c(x) str.contains(x, Qt::CaseInsensitive)
QVariant::Type SqlDBWriter::getType(const QString &str) {

    if (str.isEmpty() || c(" BINARY") || c(" BLOB") || c(" TINYBLOB") || c(" MEDIUMBLOB") || c(" LONGBLOB")) {
        return QVariant::ByteArray;
    } else if (c(" INT")) {
        return QVariant::Int;
    } else if (c(" VARCHAR") || c(" TEXT") || c(" TINYTEXT") || c(" MEDIUMTEXT") || c(" LONGTEXT")) {
        return QVariant::String;
    } else if (c(" FLOAT") || c(" DOUBLE") || c(" REAL")) {
        return QVariant::Double;
    } else if (c(" BOOL")) {
        return QVariant::Bool;
    } else if (c(" DATETIME")) {
        return QVariant::DateTime;
    } else if (c(" DATE")) {
        return QVariant::Date;
    } else if (c(" TIME")) {
        return QVariant::Time;
    }

    return QVariant::ByteArray;
}

bool SqlDBWriter::generateHeaderOfQuery(QString& retQuery,
                                        const DbTableBase &tableStruct) const {

    retQuery = "";
    if (!tableStruct.isValid()) {
        return false;
    }

    for (auto it = tableStruct.keys.begin(); it != tableStruct.keys.end(); ++it) {
        retQuery += it.key() + ", ";
    }

    return true;
}

bool SqlDBWriter::generateSourceOfQuery(QString &retQuery,
                                        QList<QPair<QString, QVariant> > &retBindValue,
                                        const DbTableBase &tableStruct,
                                        const QVariantMap& dataMap) const {

    retQuery = "";
    retBindValue.clear();

    if (!tableStruct.isValid()) {
        return false;
    }

    for (auto it = tableStruct.keys.begin(); it != tableStruct.keys.end(); ++it) {
        auto type = it.value();
        if (type != QVariant::UserType) {

            switch (type) {
            case QVariant::String:
            case QVariant::Int:
            case QVariant::Double:  {
                retQuery += "'" + dataMap.value(it.key()).toString() + "', ";
                break;
            }
            case QVariant::Time: {
                retQuery += "'" + dataMap.value(it.key()).toDate().toString("HH:mm:ss") + "', ";
                break;

            }
            case QVariant::DateTime: {
                retQuery += "'" + dataMap.value(it.key()).toDateTime().toString("yyyy-MM-dd HH:mm:ss") + "', ";
                break;
            }
            case QVariant::Date: {
                retQuery += "'" + dataMap.value(it.key()).toDate().toString("yyyy-MM-dd") + "', ";
                break;
            }
            case QVariant::ByteArray: {
                auto bindValue = it.key() + "bytes";
                retQuery += ":" + bindValue + ", ";
                retBindValue.push_back({bindValue, dataMap.value(it.key())});

                break;
            }
            default: {
                break;
            }

            }

        }
    }

    return true;
}

bool SqlDBWriter::getBaseQueryString(QString queryString, QSqlQuery *query,
                                     const DbTableBase& tableStruct,
                                     const QVariantMap& objMap) const {

    if (!tableStruct.isValid()) {
        return false;
    }

    queryString = queryString.arg(tableStruct.name);

    QString temp = "";
    if (!generateHeaderOfQuery(temp, tableStruct)) {
        return false;
    }

    queryString = queryString.arg(temp);

    if (objMap.isEmpty()) {
        // prepare only header
        return true;
    }

    temp = "";
    QList<QPair<QString, QVariant> > bindValues;

    if (!generateSourceOfQuery(temp, bindValues, tableStruct, objMap)) {
        return false;
    }

    queryString = queryString.arg(temp);

    if (!query->prepare(queryString)) {
        return false;
    }

    for (auto &i: bindValues) {
        query->bindValue(i.first, i.second);
    }

    return true;
}

bool SqlDBWriter::saveQuery(const QWeakPointer<DBObject>& ptr) const {

    QSqlQuery q(db);

    QString queryString = "INSERT IGNORE INTO %0(%1) VALUES (%2)";

    auto obj = ptr.toStrongRef();
    if (obj.isNull())
        return false;

    if (!getBaseQueryString(queryString, &q,
                            _dbStruct.value(obj->tableName()),
                            obj->getMap())) {
        return false;
    }

    return q.exec();
}

bool SqlDBWriter::selectQuery(QList<QSharedPointer<DBObject>>& returnList,
                              const QString& table,
                              const QString &key,
                              const QVariant &val) {

    if (!val.isValid()) {
        return false;
    }



    QString queryString;
    if (key == "id") {
        queryString = "SELECT (%1) from %0 where id=" + val.toString();
    } else {
        queryString = "SELECT * from %0 where " + key + "='" + val.toString() + "'";
    }

    QSqlQuery query(db);

    auto tableStruct = _dbStruct.value(table);

    if (!getBaseQueryString(queryString, &query,
                            tableStruct)) {
        return false;
    }

    if (!query.exec()) {
        return false;
    }

    QSqlRecord record = query.record();

    returnList.clear();
    while (query.next()) {
        QVariantMap initMap;

        for (int i = 0; i < record.count(); ++i ) {
            initMap[record.fieldName(i)] = record.value(i);
        }

        auto obj = DbObjectsFactory::factory(table);
        if (!checkTableStruct(obj)) {

            QuasarAppUtils::Params::verboseLog("object from factory have non valid structure",
                                               QuasarAppUtils::Error);
            continue;
        }

        obj->setMap(initMap);

        if (obj->isValid())
            returnList.push_back(obj);

    }
    return returnList.size();
}

bool SqlDBWriter::deleteQuery(const QString& table, int id) const {

    QString queryString = "DELETE FROM %0 where id=" + QString::number(id);
    QSqlQuery query(db);

    if (!getBaseQueryString(queryString, &query, _dbStruct.value(table))) {
        return false;
    }

    return query.exec();
}

bool SqlDBWriter::checkTableStruct(const QWeakPointer<DBObject> &ptr) {
    auto obj = ptr.toStrongRef();

    if (obj.isNull() || !obj->isValid())
        return false;

    auto map = obj->getMap();
    auto tableStruct = _dbStruct.value(obj->tableName());

    if (!tableStruct.isValid()) {
        return false;
    }

    for (auto it = tableStruct.keys.begin(); it != tableStruct.keys.end(); ++it) {
        if (!map.contains(it.key())) {
            return false;
        }
    }

    return true;
}

}
