/*
 * Copyright (C) 2018-2020 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
*/

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
#include <QStandardPaths>
#include <QCoreApplication>

namespace NP {

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
                        QuasarAppUtils::Params::log("exec database error: " +sq->lastError().text(),
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
        QuasarAppUtils::Params::log("request error : " + query.lastError().text());
        return false;
    }

    return true;
}

bool SqlDBWriter::disableFK() {

    QSqlQuery query(db);
    QString request = QString("PRAGMA foreign_keys = OFF");
    if (!query.exec(request)) {
        QuasarAppUtils::Params::log("request error : " + query.lastError().text());
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
        QuasarAppUtils::Params::log(db.lastError().text(),
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

bool SqlDBWriter::getAllObjects(const DBObject &templateObject,  QList<DBObject *> &result) {
    return selectQuery(templateObject, result);
}

bool SqlDBWriter::saveObject(const DBObject* ptr) {
    return saveQuery(ptr);
}

bool SqlDBWriter::deleteObject(const DBObject* ptr) {
    return deleteQuery(ptr);
}

SqlDBWriter::~SqlDBWriter() {
}

bool SqlDBWriter::saveQuery(const DBObject* ptr) const {
    if (ptr)
        return false;

    QSqlQuery q(db);

    auto prepare = [ptr](QSqlQuery&q) {
        return ptr->prepareSaveQuery(q);
    };

    auto cb = [](){return true;};

    return workWithQuery(q, prepare, cb);
}

bool SqlDBWriter::selectQuery(const DBObject& requestObject, QList<DBObject *> &result) {

    QSqlQuery q(db);
    auto prepare = [&requestObject](QSqlQuery&q) {
        return requestObject.prepareSelectQuery(q);
    };

    auto cb = [&q, &requestObject, &result]() -> bool {
        while (q.next()) {
            auto newObject = requestObject.factory();

            if (!newObject)
                return false;

            if (!newObject->fromSqlRecord(q.record())) {
                QuasarAppUtils::Params::log("Init sql object error.",
                                            QuasarAppUtils::Error);
                return false;
            }
            result.push_back(newObject);
        }

        return result.size();
    };

    return workWithQuery(q, prepare, cb);
}

bool SqlDBWriter::deleteQuery(const DBObject *deleteObject) const {
    if (deleteObject)
        return false;

    QSqlQuery q(db);

    auto prepare = [deleteObject](QSqlQuery&q) {
        return deleteObject->prepareRemoveQuery(q);
    };

    auto cb = []() -> bool {
        return true;
    };

    return workWithQuery(q, prepare, cb);
}

bool SqlDBWriter::workWithQuery(QSqlQuery &q,
                               const std::function< PrepareResult (QSqlQuery &)> &prepareFunc,
                               const std::function<bool ()> &cb) const {

    switch (prepareFunc(q)) {
    case PrepareResult::Success: {

        if (!q.exec()) {

            QuasarAppUtils::Params::log("exec sql error: " + q.lastError().text(),
                                        QuasarAppUtils::Error);

            return false;
        }

        return cb();
    }
    case PrepareResult::Disabled: {
        QuasarAppUtils::Params::log("call disabled operator! ",
                                    QuasarAppUtils::Warning);
        return true;
    }

    case PrepareResult::Fail: {
        QuasarAppUtils::Params::log("prepare sql error: " + q.lastError().text(),
                                    QuasarAppUtils::Error);

        return false;
    }

    }

    return false;

}

}
