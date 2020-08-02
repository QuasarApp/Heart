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
    _mainThreadId = QThread::currentThreadId();

}

bool AsyncSqlDbWriter::saveObject(const DBObject *saveObject) {
    if (QThread::currentThreadId() == _mainThreadId) {
        return SqlDBWriter::saveObject(saveObject);
    }

    return QMetaObject::invokeMethod(this,
                                     "handleSaveObject",
                                     Qt::QueuedConnection,
                                     Q_ARG(const DBObject *,
                                           saveObject));

}

bool AsyncSqlDbWriter::deleteObject(const DBObject *deleteObject) {
    if (QThread::currentThreadId() == _mainThreadId) {
        return SqlDBWriter::deleteObject(deleteObject);
    }

    return QMetaObject::invokeMethod(this,
                                     "handleDeleteObject",
                                     Qt::QueuedConnection,
                                     Q_ARG(const DBObject *,
                                           deleteObject));
}

bool AsyncSqlDbWriter::getAllObjects(const DBObject &templateObject, QList<DBObject *> &result) {

    if (QThread::currentThreadId() == _mainThreadId) {
        return SqlDBWriter::getAllObjects(templateObject, result);
    }

    bool workOfEnd = false, workResult = false;
    bool invockeResult = QMetaObject::invokeMethod(this,
                                                   "handleDeleteObject",
                                                   Qt::QueuedConnection,
                                                   Q_ARG(const DBObject &, templateObject),
                                                   Q_ARG(QList<DBObject *> &, result),
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

void AsyncSqlDbWriter::handleGetAllObject(const DBObject &templateObject, QList<DBObject *> &result,
                                          bool *resultOfWork, bool *endOfWork) {


    *resultOfWork = SqlDBWriter::getAllObjects(templateObject, result);

    if (endOfWork) {
        *endOfWork = true;
    }

}

bool AsyncSqlDbWriter::waitFor(bool *condition, int timeout) const {
    auto curmsec = QDateTime::currentMSecsSinceEpoch() + timeout;
    while (curmsec > QDateTime::currentMSecsSinceEpoch() && *condition) {
        QCoreApplication::processEvents();
    }
    QCoreApplication::processEvents();
    return *condition;
}

}
