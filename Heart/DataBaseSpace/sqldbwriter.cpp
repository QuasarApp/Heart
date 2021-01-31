/*
 * Copyright (C) 2018-2021 QuasarApp.
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
#include <QJsonDocument>
#include <QJsonObject>
#include <QHash>
#include <dbobject.h>
#include <QSqlRecord>
#include <QStandardPaths>
#include <QCoreApplication>

namespace QH {
using namespace PKG;

bool SqlDBWriter::exec(QSqlQuery *sq,const QString& sqlFile) {
    QFile f(sqlFile);
    bool result = true;
    if (f.open(QIODevice::ReadOnly)) {

        QString temp, delimiter = ";";
        QTextStream stream(&f);
#if QT_VERSION < QT_VERSION_CHECK(6,0,0)
        stream.setCodec("UTF8");
#endif

        while(!stream.atEnd()) {
            QString line = stream.readLine();
            int linedelimiterIndex = line.lastIndexOf(delimiter, -1, Qt::CaseInsensitive);
            int delimiterIndex = temp.size() + linedelimiterIndex;

            if (line.contains("--"))
                continue;

            temp += line;

            if (linedelimiterIndex > -1) {
                result = result && sq->exec(temp.left(delimiterIndex));
                temp = temp.remove(0, delimiterIndex + 1);

                if (!result) {
                    QuasarAppUtils::Params::log("exec database error: " + sq->lastError().text(),
                                                QuasarAppUtils::Error);
                    f.close();
                    return false;
                }
            }
        }

        f.close();
        return result;
    }

    QuasarAppUtils::Params::log("sql source file is not open: " + sqlFile,
                                QuasarAppUtils::Error);

    return false;
}

bool SqlDBWriter::initDbPrivate(const QVariantMap &params) {
    _config = params;

    if (_db)
        delete _db;

    _db = new QSqlDatabase(initSqlDataBasse(_config["DBDriver"].toString(),
            _config["DBFilePath"].toString()));

    if (_config.contains("DBFilePath")) {

        auto path = QFileInfo(_config["DBFilePath"].toString());
        if (!QDir("").mkpath(path.absolutePath())) {
            return false;
        }

        _db->setDatabaseName(path.absoluteFilePath());
    }

    if (_config.contains("DBLogin")) {
        _db->setPassword(_config["DBLogin"].toString());
    }

    if (_config.contains("DBPass")) {
        _db->setPassword(_config["DBPass"].toString());
    }

    if (_config.contains("DBHost")) {
        _db->setHostName(_config["DBHost"].toString());
    }

    if (_config.contains("DBPort")) {
        _db->setPort(_config["DBPort"].toInt());
    }

    if (!_db->open()) {
        QuasarAppUtils::Params::log(_db->lastError().text(),
                                    QuasarAppUtils::Error);
        return false;
    }

    for (const QString& sqlFile : qAsConst(_SQLSources)) {
        QSqlQuery query(*_db);
        if (!exec(&query, sqlFile)) {
            return false;
        }
    }

    if (_config.contains("DBInitFile")) {
        auto path = QFileInfo(_config["DBInitFile"].toString()).absoluteFilePath();

        QSqlQuery query(*_db);
        if (!exec(&query, path)) {
            return false;
        }
    }

    initSuccessful = _db->isValid();
    return initSuccessful;
}

bool SqlDBWriter::enableFK() {
    if (!db()) {
        return false;
    }

    QSqlQuery query(*db());
    QString request = QString("PRAGMA foreign_keys = ON");
    if (!query.exec(request)) {
        QuasarAppUtils::Params::log("request error : " + query.lastError().text());
        return false;
    }

    return true;
}

bool SqlDBWriter::disableFK() {

    if (!db()) {
        return false;
    }

    QSqlQuery query(*db());
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

QSqlDatabase SqlDBWriter::initSqlDataBasse(const QString& driverName,
                                           const QString& name) {

    return QSqlDatabase::addDatabase(driverName,
                                     QFileInfo(name).fileName());
}

QSqlDatabase *SqlDBWriter::db() {
    return _db;
}

const QSqlDatabase *SqlDBWriter::db() const {
    return _db;
}

SqlDBWriter::SqlDBWriter(QObject* ptr):
    Async(ptr) {
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
    auto handleInitDb = [&params, this]() {
        return initDbPrivate(params);
    };

    return asyncLauncher(handleInitDb, true);
}

bool SqlDBWriter::isValid() const {
    return db() && db()->isValid() && db()->isOpen() && initSuccessful;
}

bool SqlDBWriter::getAllObjects(const DBObject &templateObject,
                                QList<QSharedPointer<DBObject>> &result) {

    auto getAll = [&templateObject, &result, this]() {
        return SqlDBWriter::selectQuery(templateObject, result);
    };

    return asyncLauncher(getAll, true);
}

bool SqlDBWriter::updateObject(const QSharedPointer<DBObject> &ptr, bool wait) {

    Async::Job job = [this, ptr]() {
        return updateQuery(ptr);
    };

    return asyncLauncher(job, wait);
}

bool SqlDBWriter::deleteObject(const QSharedPointer<DBObject> &ptr, bool wait) {

    Async::Job job = [this, ptr]() {
        return deleteQuery(ptr);
    };

    return asyncLauncher(job, wait);
}

bool SqlDBWriter::insertObject(const QSharedPointer<DBObject> &ptr, bool wait) {

    Async::Job job = [this, ptr]() {
        return insertQuery(ptr);
    };

    return asyncLauncher(job, wait);

}

void SqlDBWriter::setSQLSources(const QStringList &list) {
    _SQLSources = list;
}

QString SqlDBWriter::databaseLocation() const {
    if (!db())
        return "";

    return db()->databaseName();
}

SqlDBWriter::~SqlDBWriter() {
    if (_db) {
        _db->close();

        QString connectionName = _db->connectionName();

        delete _db;
        _db = nullptr;

        QSqlDatabase::removeDatabase(connectionName);
    }

}

bool SqlDBWriter::insertQuery(const QSharedPointer<DBObject> &ptr) const {
    if (!ptr)
        return false;

    if (!db()) {
        return false;
    }

    QSqlQuery q(*db());

    auto prepare = [ptr](QSqlQuery&q) {
        return ptr->prepareInsertQuery(q);
    };

    auto cb = [](){return true;};

    return workWithQuery(q, prepare, cb);
}

bool SqlDBWriter::selectQuery(const DBObject& requestObject,
                              QList<QSharedPointer<QH::PKG::DBObject>> &result) {

    if (!db()) {
        return false;
    }

    QSqlQuery q(*db());
    auto prepare = [&requestObject](QSqlQuery&q) {
        return requestObject.prepareSelectQuery(q);
    };

    auto cb = [&q, &requestObject, &result]() -> bool {

        if (requestObject.isBundle()) {
            auto newObject = QSharedPointer<QH::PKG::DBObject>(requestObject.createDBObject());

            if (!newObject)
                return false;

            while (q.next()) {
                if (!newObject->fromSqlRecord(q.record())) {
                    QuasarAppUtils::Params::log("Init sql object error.",
                                                QuasarAppUtils::Error);
                    return false;
                }
            }

            result.push_back(newObject);

        } else {
            while (q.next()) {
                auto newObject = QSharedPointer<QH::PKG::DBObject>(requestObject.createDBObject());

                if (!newObject)
                    return false;

                if (!newObject->fromSqlRecord(q.record())) {
                    QuasarAppUtils::Params::log("Init sql object error.",
                                                QuasarAppUtils::Error);
                    return false;
                }
                result.push_back(newObject);
            }
        }

        return result.size();
    };

    return workWithQuery(q, prepare, cb);
}

bool SqlDBWriter::deleteQuery(const QSharedPointer<DBObject> &deleteObject) const {
    if (!deleteObject)
        return false;

    QSqlQuery q(*db());

    auto prepare = [deleteObject](QSqlQuery&q) {
        return deleteObject->prepareRemoveQuery(q);
    };

    auto cb = []() -> bool {
        return true;
    };


    return workWithQuery(q, prepare, cb);
}

bool SqlDBWriter::updateQuery(const QSharedPointer<DBObject> &ptr) const {
    if (!ptr)
        return false;

    QSqlQuery q(*db());

    auto prepare = [ptr](QSqlQuery&q) {
        return ptr->prepareUpdateQuery(q);
    };

    auto cb = [](){return true;};

    return workWithQuery(q, prepare, cb);
}

bool SqlDBWriter::workWithQuery(QSqlQuery &q,
                                const std::function< PrepareResult (QSqlQuery &)> &prepareFunc,
                                const std::function<bool ()> &cb) const {

    auto erroPrint = [](const QSqlQuery &q){
        QuasarAppUtils::Params::log("exec sql error: " + q.lastError().text(),
                                    QuasarAppUtils::Error);

        QuasarAppUtils::Params::log("prepare sql error: " + q.executedQuery(),
                                    QuasarAppUtils::Error);
    };


    switch (prepareFunc(q)) {
    case PrepareResult::Success: {

        if (!q.exec()) {
            erroPrint(q);
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
        erroPrint(q);
        return false;
    }

    }

    return false;

}

}
