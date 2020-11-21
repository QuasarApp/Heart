/*
 * Copyright (C) 2018-2020 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
*/

#include "promise.h"
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
#include <QThread>
#include <functional>
#include <execution>

namespace QH {
using namespace PKG;

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
                        QuasarAppUtils::Params::log("exec database error: " + sq->lastError().text(),
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

bool SqlDBWriter::initDbPrivate(const QVariantMap &params) {
    _config = params;

    db = QSqlDatabase::addDatabase(_config["DBDriver"].toString(),
            QFileInfo(_config["DBFilePath"].toString()).fileName());

    if (_config.contains("DBFilePath")) {

        auto path = QFileInfo(_config["DBFilePath"].toString());
        if (!QDir("").mkpath(path.absolutePath())) {
            return false;
        }

        db.setDatabaseName(path.absoluteFilePath());
    }

    if (_config.contains("DBLogin")) {
        db.setPassword(_config["DBLogin"].toString());
    }

    if (_config.contains("DBPass")) {
        db.setPassword(_config["DBPass"].toString());
    }

    if (_config.contains("DBHost")) {
        db.setHostName(_config["DBHost"].toString());
    }

    if (_config.contains("DBPort")) {
        db.setPort(_config["DBPort"].toInt());
    }

    if (!db.open()) {
        QuasarAppUtils::Params::log(db.lastError().text(),
                                    QuasarAppUtils::Error);
        return false;
    }

    for (const auto& sqlFile : _SQLSources) {
        QSqlQuery query(db);
        if (!exec(&query, sqlFile)) {
            return false;
        }
    }

    if (_config.contains("DBInitFile")) {
        auto path = QFileInfo(_config["DBInitFile"].toString()).absoluteFilePath();

        QSqlQuery query(db);
        if (!exec(&query, path)) {
            return false;
        }
    }

    initSuccessful = db.isValid();
    return initSuccessful;
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

SqlDBWriter::SqlDBWriter(QObject* ptr):
    QObject(ptr) {
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
    if (QThread::currentThread() == thread()) {
        return SqlDBWriter::initDb(params);
    }

    bool workOfEnd = false, workResult = false;


    bool invockeResult = QMetaObject::invokeMethod(this,
                                                   "handleInitDb",
                                                   Qt::QueuedConnection,
                                                   Q_ARG(const QVariantMap &, params),
                                                   Q_ARG(bool *, &workResult),
                                                   Q_ARG(bool *, &workOfEnd));

    if (!invockeResult)
        return false;


    if (!waitFor(&workOfEnd)) {
        return false;
    }

    return workResult;
}

bool SqlDBWriter::isValid() const {
    return db.isValid() && db.isOpen() && initSuccessful;
}

bool SqlDBWriter::getAllObjects(const DBObject &templateObject,
                                Promise<QList<const DBObject *> > &result) {

    if (QThread::currentThread() == thread()) {
        return SqlDBWriter::selectQuery(templateObject, &result);
    }

    bool invockeResult = QMetaObject::invokeMethod(this,
                                                   "selectQuery",
                                                   Qt::QueuedConnection,
                                                   Q_ARG(const QH::PKG::DBObject *, &templateObject),
                                                   Q_ARG(Promise<QList<const QH::PKG::DBObject *>>*, &result));


    return invockeResult;
}

bool SqlDBWriter::updateObject(const DBObject* ptr) {
    if (QThread::currentThread() == thread()) {
        return updateQuery(ptr, nullptr, nullptr);
    }

    auto clone = ptr->cloneRaw();

    bool workOfEnd = false, workResult = false;

    bool invockeResult = QMetaObject::invokeMethod(this,
                                                   "updateQuery",
                                                   Qt::QueuedConnection,
                                                   Q_ARG(const QH::PKG::DBObject *, clone),
                                                   Q_ARG(bool *, &workResult),
                                                   Q_ARG(bool *, &workOfEnd));

    delete clone;

    if (!invockeResult)
        return false;


    if (!waitFor(&workOfEnd)) {
        return false;
    }

    return workResult;
}

bool SqlDBWriter::deleteObject(const DBObject* ptr) {
    if (QThread::currentThread() == thread()) {
        return deleteQuery(ptr, nullptr, nullptr);
    }

    bool workOfEnd = false, workResult = false;
    auto clone = ptr->cloneRaw();

    bool invockeResult = QMetaObject::invokeMethod(this,
                                                   "deleteQuery",
                                                   Qt::QueuedConnection,
                                                   Q_ARG(const QH::PKG::DBObject *, clone));
    delete clone;

    if (!invockeResult)
        return false;


    if (!waitFor(&workOfEnd)) {
        return false;
    }

    return workResult;
}

bool SqlDBWriter::insertObject(const DBObject *saveObject) {

    if (QThread::currentThread() == thread()) {
        return insertQuery(saveObject, nullptr, nullptr);
    }

    bool workOfEnd = false, workResult = false;
    auto clone = saveObject->cloneRaw();

    bool invockeResult = QMetaObject::invokeMethod(this,
                                                   "insertQuery",
                                                   Qt::QueuedConnection,
                                                   Q_ARG(const QH::PKG::DBObject *, clone));
    delete clone;

    if (!invockeResult)
        return false;


    if (!waitFor(&workOfEnd)) {
        return false;
    }

    return workResult;

}

bool SqlDBWriter::waitFor(bool *condition, int timeout) const {
    auto curmsec = QDateTime::currentMSecsSinceEpoch() + timeout;
    while (curmsec > QDateTime::currentMSecsSinceEpoch() && !*condition) {
        QCoreApplication::processEvents();
    }
    QCoreApplication::processEvents();
    return *condition;
}

void SqlDBWriter::handleInitDb(const QVariantMap &params, bool *resultOfWork, bool *endOfWork) {

    bool work = initDbPrivate(params);

    if (resultOfWork) {
        *resultOfWork = work;
    }

    if (endOfWork) {
        *endOfWork = true;
    }
}


bool SqlDBWriter::updateObjectWithWait(const DBObject *saveObject) {
    if (QThread::currentThread() == thread()) {
        return SqlDBWriter::updateObject(saveObject);
    }

    bool workOfEnd = false, workResult = false;


    bool invoke = QMetaObject::invokeMethod(this,
                                            "handleUpdateObject",
                                            Qt::QueuedConnection,
                                            Q_ARG(const QH::PKG::DBObject *, saveObject),
                                            Q_ARG(bool *, &workResult),
                                            Q_ARG(bool *, &workOfEnd));

    if (!invoke) {
        QuasarAppUtils::Params::log("handleUpdateObject not invokecd", QuasarAppUtils::Debug);
        return false;
    }


    if (!waitFor(&workOfEnd)) {
        return false;
    }

    return workResult;
}

bool SqlDBWriter::insertObjectWithWait(const DBObject *saveObject) {
    if (QThread::currentThread() == thread()) {
        return SqlDBWriter::insertObject(saveObject);
    }

    bool workOfEnd = false, workResult = false;


    bool invoke = QMetaObject::invokeMethod(this,
                                            "handleInsertObject",
                                            Qt::QueuedConnection,
                                            Q_ARG(const QH::PKG::DBObject *, saveObject),
                                            Q_ARG(bool *, &workResult),
                                            Q_ARG(bool *, &workOfEnd));

    if (!invoke) {
        QuasarAppUtils::Params::log("handleInsertObject not invokecd", QuasarAppUtils::Debug);
        return false;
    }


    if (!waitFor(&workOfEnd)) {
        return false;
    }

    return workResult;
}

bool SqlDBWriter::deleteObjectWithWait(const DBObject *deleteObject) {
    if (QThread::currentThread() == thread()) {
        return SqlDBWriter::deleteObject(deleteObject);
    }

    bool workOfEnd = false, workResult = false;


    bool invoke = QMetaObject::invokeMethod(this,
                                            "handleDeleteObject",
                                            Qt::QueuedConnection,
                                            Q_ARG(const QH::PKG::DBObject *, deleteObject),
                                            Q_ARG(bool *, &workResult),
                                            Q_ARG(bool *, &workOfEnd));

    if (!invoke) {
        QuasarAppUtils::Params::log("handleDeleteObject not invokecd", QuasarAppUtils::Debug);
        return false;
    }


    if (!waitFor(&workOfEnd)) {
        return false;
    }

    return workResult;
}

void SqlDBWriter::setSQLSources(const QStringList &list) {
    _SQLSources = list;
}

QString SqlDBWriter::databaseLocation() const {
    return db.databaseName();
}

SqlDBWriter::~SqlDBWriter() {
    db.close();
}

bool SqlDBWriter::insertQuery(const DBObject* ptr,
                              bool *workResult, bool *workOfEnd) const {
    if (!ptr)
        return false;

    QSqlQuery q(db);

    auto prepare = [ptr](QSqlQuery&q) {
        return ptr->prepareInsertQuery(q);
    };

    auto cb = [](){return true;};

    bool work = workWithQuery(q, prepare, cb);
    if (workResult)
        *workResult = work;

    if (workOfEnd) {
        *workOfEnd = true;
    }

    return work;
}

bool SqlDBWriter::selectQuery(const DBObject& requestObject,
                              Promise<QList<const DBObject *>> *result) {

    if(!result)
        return false;

    QSqlQuery q(db);
    auto prepare = [&requestObject](QSqlQuery&q) {
        return requestObject.prepareSelectQuery(q);
    };

    QList<const DBObject *> res;
    auto cb = [&q, &requestObject, &res]() -> bool {

        if (requestObject.isBundle()) {
            auto newObject = requestObject.createDBObject();

            if (!newObject)
                return false;

            while (q.next()) {
                if (!newObject->fromSqlRecord(q.record())) {
                    QuasarAppUtils::Params::log("Init sql object error.",
                                                QuasarAppUtils::Error);
                    delete newObject;
                    return false;
                }
            }

            res.push_back(newObject);

        } else {
            while (q.next()) {
                auto newObject = requestObject.createDBObject();

                if (!newObject)
                    return false;

                if (!newObject->fromSqlRecord(q.record())) {
                    QuasarAppUtils::Params::log("Init sql object error.",
                                                QuasarAppUtils::Error);
                    delete newObject;
                    continue;
                }
                res.push_back(newObject);
            }
        }

        return res.size();
    };

    if(!workWithQuery(q, prepare, cb)) {
        result->reject();
        return false;
    }

    result->setValue(res);

    return true;
}

bool SqlDBWriter::deleteQuery(const DBObject *deleteObject,
                              bool *workResult, bool *workOfEnd) const {
    if (!deleteObject)
        return false;

    QSqlQuery q(db);

    auto prepare = [deleteObject](QSqlQuery&q) {
        return deleteObject->prepareRemoveQuery(q);
    };

    auto cb = []() -> bool {
            return true;
};

    bool work = workWithQuery(q, prepare, cb);
    if (workResult)
        *workResult = work;

    if (workOfEnd) {
        *workOfEnd = true;
    }

    return work;
}

bool SqlDBWriter::updateQuery(const DBObject *ptr,
                              bool *workResult, bool *workOfEnd) const {
    if (!ptr)
        return false;

    QSqlQuery q(db);

    auto prepare = [ptr](QSqlQuery&q) {
        return ptr->prepareUpdateQuery(q);
    };

    auto cb = [](){return true;};

    bool work = workWithQuery(q, prepare, cb);
    if (workResult)
        *workResult = work;

    if (workOfEnd) {
        *workOfEnd = true;
    }

    return work;
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
