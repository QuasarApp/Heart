/*
 * Copyright (C) 2018-2020 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
*/

#include "asyncsqldbwriter.h"

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
}

bool AsyncSqlDbWriter::saveObject(const WP<AbstractData>& saveObject) {
    auto obj = saveObject.toStrongRef();
    auto z = obj.dynamicCast<DBObject>();
    return QMetaObject::invokeMethod(this,
                              "handleSaveObject",
                              Qt::QueuedConnection,
                              Q_ARG(SP<DBObject>,
                                    z));

}

bool AsyncSqlDbWriter::deleteObject(const WP<AbstractData>& deleteObject) {
    return QMetaObject::invokeMethod(this,
                               "handleDeleteObject",
                               Qt::QueuedConnection,
                               Q_ARG(SP<DBObject>,
                                     deleteObject.toStrongRef().dynamicCast<DBObject>()));
}

void AsyncSqlDbWriter::handleSaveObject(SP<DBObject> saveObject) {
    if (!SqlDBWriter::saveObject(saveObject)) {
        QuasarAppUtils::Params::log("AsyncSqlDbWriter: save object fail!",
                                           QuasarAppUtils::Error);
    }
}

void AsyncSqlDbWriter::handleDeleteObject(SP<DBObject> deleteObject) {
    if (!SqlDBWriter::deleteObject(deleteObject)) {
        QuasarAppUtils::Params::log("AsyncSqlDbWriter: delete object fail!",
                                           QuasarAppUtils::Error);
    }
}

}
