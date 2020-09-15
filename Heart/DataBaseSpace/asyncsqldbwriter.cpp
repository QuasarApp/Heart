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

namespace QH {


AsyncSqlDbWriter::AsyncSqlDbWriter(QObject *ptr):
    QObject(ptr) {
    _own = new QThread(this);
    moveToThread(_own);
    _own->start();

}

AsyncSqlDbWriter::~AsyncSqlDbWriter() {
    _own->quit();
    _own->wait();
}

bool AsyncSqlDbWriter::saveObject(const DBObject *saveObject) {
    if (QThread::currentThread() == thread()) {
        return SqlDBWriter::saveObject(saveObject);
    }

    return QMetaObject::invokeMethod(this,
                                     "handleSaveObject",
                                     Qt::QueuedConnection,
                                     Q_ARG(const QH::DBObject *, saveObject),
                                     Q_ARG(bool *, nullptr),
                                     Q_ARG(bool *, nullptr));

}

bool AsyncSqlDbWriter::deleteObject(const DBObject *deleteObject) {
    if (QThread::currentThread() == thread()) {
        return SqlDBWriter::deleteObject(deleteObject);
    }

    return QMetaObject::invokeMethod(this,
                                     "handleDeleteObject",
                                     Qt::QueuedConnection,
                                     Q_ARG(const QH::DBObject *, deleteObject),
                                     Q_ARG(bool *, nullptr),
                                     Q_ARG(bool *, nullptr));
}

bool AsyncSqlDbWriter::saveObjectWithWait(const DBObject *saveObject) {
    if (QThread::currentThread() == thread()) {
        return SqlDBWriter::saveObject(saveObject);
    }

    bool workOfEnd = false, workResult = false;


    bool invoke = QMetaObject::invokeMethod(this,
                                     "handleSaveObject",
                                     Qt::QueuedConnection,
                                     Q_ARG(const QH::DBObject *, saveObject),
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

bool AsyncSqlDbWriter::deleteObjectWithWait(const DBObject *deleteObject) {
    if (QThread::currentThread() == thread()) {
        return SqlDBWriter::deleteObject(deleteObject);
    }

    bool workOfEnd = false, workResult = false;


    bool invoke = QMetaObject::invokeMethod(this,
                                     "handleDeleteObject",
                                     Qt::QueuedConnection,
                                     Q_ARG(const QH::DBObject *, deleteObject),
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

bool AsyncSqlDbWriter::getAllObjects(const DBObject &templateObject, QList<const DBObject *> &result) {

    if (QThread::currentThread() == thread()) {
        return SqlDBWriter::getAllObjects(templateObject, result);
    }

    bool workOfEnd = false, workResult = false;


    bool invockeResult = QMetaObject::invokeMethod(this,
                                                   "handleGetAllObject",
                                                   Qt::QueuedConnection,
                                                   Q_ARG(const QH::DBObject *, &templateObject),
                                                   Q_ARG(QList<const QH::DBObject *> *, &result),
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

void AsyncSqlDbWriter::handleSaveObject(const DBObject* saveObject,
                                        bool *resultOfWork,
                                        bool *endOfWork) {
    if (resultOfWork) {
        *resultOfWork = SqlDBWriter::saveObject(saveObject);

        if (endOfWork) {
            *endOfWork = true;
        }

    } else {
        if (!SqlDBWriter::saveObject(saveObject)) {
            QuasarAppUtils::Params::log("AsyncSqlDbWriter: save object fail!",
                                        QuasarAppUtils::Error);
        }
    }
}

void AsyncSqlDbWriter::handleDeleteObject(const DBObject *deleteObject, bool *resultOfWork, bool *endOfWork) {
    if (resultOfWork) {
        *resultOfWork = SqlDBWriter::deleteObject(deleteObject);

        if (endOfWork) {
            *endOfWork = true;
        }

    } else {
        if (!SqlDBWriter::deleteObject(deleteObject)) {
            QuasarAppUtils::Params::log("AsyncSqlDbWriter: delete object fail!",
                                        QuasarAppUtils::Error);
        }
    }
}

void AsyncSqlDbWriter::handleGetAllObject(const DBObject *templateObject,
                                          QList<const DBObject *> *result,
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