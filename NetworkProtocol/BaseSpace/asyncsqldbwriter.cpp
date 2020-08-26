/*
 * Copyright (C) 2018-2020 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
*/

#include "asyncsqldbwriter.h"

#include <QDateTime>
#include <QMetaMethod>
#include <QThread>
#include <dbobject.h>
#include <quasarapp.h>

namespace NP {


AsyncSqlDbWriter::AsyncSqlDbWriter(QObject *ptr):
    QObject(ptr)
{
    QThread *own = new QThread(this);
    moveToThread(own);
    own->start();

}

bool AsyncSqlDbWriter::saveObject(const DBObject *saveObject) {
    if (QThread::currentThread() == thread()) {
        return SqlDBWriter::saveObject(saveObject);
    }

    return QMetaObject::invokeMethod(this,
                                     "handleSaveObject",
                                     Qt::QueuedConnection,
                                     Q_ARG(const NP::DBObject *,
                                           saveObject));

}

bool AsyncSqlDbWriter::deleteObject(const DBObject *deleteObject) {
    if (QThread::currentThread() == thread()) {
        return SqlDBWriter::deleteObject(deleteObject);
    }

    return QMetaObject::invokeMethod(this,
                                     "handleDeleteObject",
                                     Qt::QueuedConnection,
                                     Q_ARG(const NP::DBObject *,
                                           deleteObject));
}

bool AsyncSqlDbWriter::getAllObjects(const DBObject &templateObject, QList<DBObject *> &result) {

    if (QThread::currentThread() == thread()) {
        return SqlDBWriter::getAllObjects(templateObject, result);
    }

    bool workOfEnd = false, workResult = false;


    bool invockeResult = QMetaObject::invokeMethod(this,
                                                   "handleGetAllObject",
                                                   Qt::QueuedConnection,
                                                   Q_ARG(const NP::DBObject *, &templateObject),
                                                   Q_ARG(QList<NP::DBObject *> *, &result),
                                                   Q_ARG(bool *, &workResult),
                                                   Q_ARG(bool *, &workOfEnd));

    if (!invockeResult)
        return false;


    if (!waitFor(&workOfEnd)) {
        return false;
    }

    return workResult;
}

bool AsyncSqlDbWriter::initDb(const QVariantMap &params) {
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

void AsyncSqlDbWriter::handleSaveObject(const DBObject* saveObject) {
    if (!SqlDBWriter::saveObject(saveObject)) {
        QuasarAppUtils::Params::log("AsyncSqlDbWriter: save object fail!",
                                    QuasarAppUtils::Error);
    }
}

void AsyncSqlDbWriter::handleDeleteObject(const DBObject *deleteObject) {
    if (!SqlDBWriter::deleteObject(deleteObject)) {
        QuasarAppUtils::Params::log("AsyncSqlDbWriter: delete object fail!",
                                    QuasarAppUtils::Error);
    }
}

void AsyncSqlDbWriter::handleGetAllObject(const DBObject *templateObject, QList<DBObject *> *result,
                                          bool *resultOfWork, bool *endOfWork) {


    *resultOfWork = SqlDBWriter::getAllObjects(*templateObject, *result);

    if (endOfWork) {
        *endOfWork = true;
    }

}

void AsyncSqlDbWriter::handleInitDb(const QVariantMap &params,
                                    bool *resultOfWork, bool *endOfWork) {

    *resultOfWork = SqlDBWriter::initDb(params);

    if (endOfWork) {
        *endOfWork = true;
    }
}

bool AsyncSqlDbWriter::waitFor(bool *condition, int timeout) const {
    auto curmsec = QDateTime::currentMSecsSinceEpoch() + timeout;
    while (curmsec > QDateTime::currentMSecsSinceEpoch() && !*condition) {
        QCoreApplication::processEvents();
    }
    QCoreApplication::processEvents();
    return *condition;
}

}
