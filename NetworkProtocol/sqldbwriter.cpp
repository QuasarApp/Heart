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
        return result;
    }
    return false;
}

bool SqlDBWriter::enableFK() {
    QSqlQuery query(db);
    QString request = QString("PRAGMA foreign_keys = ON");
    if (!query.exec(request)) {
        QuasarAppUtils::Params::verboseLog("request error : " + query.lastError().text());
        return false;
    }

    return true;
}

bool SqlDBWriter::disableFK() {

    QSqlQuery query(db);
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

    if (!db.open()) {
        QuasarAppUtils::Params::verboseLog(db.lastError().text(),
                                           QuasarAppUtils::Error);
        return false;
    }

    if (params.contains("DBInitFile")) {
        auto path = QFileInfo(params["DBInitFile"].toString()).absoluteFilePath();

        QSqlQuery query(db);
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

bool SqlDBWriter::getObject(QWeakPointer<DBObject> obj) {
    return selectQuery(obj);
}

bool SqlDBWriter::saveObject(QSharedPointer<DBObject> saveObject) {
    return saveQuery(saveObject);
}

bool SqlDBWriter::deleteObject(QSharedPointer<DBObject> deleteObject) {
    return deleteQuery(deleteObject);
}

SqlDBWriter::~SqlDBWriter() {
}

bool SqlDBWriter::saveQuery(const QWeakPointer<DBObject>& ptr) const {QSharedPointer

    QSqlQuery q(db);

    auto obj = ptr.toStrongRef();
    if (obj.isNull())
        return false;

    if (obj->save(q)) {
        return false;
    }

    return true;
}

bool SqlDBWriter::selectQuery(QWeakPointer<DBObject>& obj) {
    auto ref = obj.toStrongRef();

    if (ref.isNull())
        return false;

    QSqlQuery query(db);
    return ref->remove(query);
}

bool SqlDBWriter::deleteQuery(QSharedPointer<DBObject> deleteObject) const {
    QSqlQuery query(db);
    return deleteObject->remove(query);
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
