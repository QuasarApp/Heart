#include "asyncsqldbwriter.h"

#include <QMetaMethod>
#include <QThread>
#include <quasarapp.h>

namespace NetworkProtocol {


AsyncSqlDbWriter::AsyncSqlDbWriter(QObject *ptr):
    QObject(ptr)
{
    QThread *own = new QThread(this);
    moveToThread(own);
}

bool AsyncSqlDbWriter::saveObject(const QWeakPointer<DBObject>& saveObject) {
   return QMetaObject::invokeMethod(this,
                              "handleSaveObject",
                              Qt::QueuedConnection,
                              Q_ARG(QSharedPointer<DBObject>, saveObject.toStrongRef()));

}

bool AsyncSqlDbWriter::deleteObject(const QWeakPointer<DBObject>& deleteObject) {
    return QMetaObject::invokeMethod(this,
                               "handleDeleteObject",
                               Qt::QueuedConnection,
                               Q_ARG(QSharedPointer<DBObject>, deleteObject.toStrongRef()));
}

void AsyncSqlDbWriter::handleSaveObject(QSharedPointer<DBObject> saveObject) {
    if (!SqlDBWriter::saveObject(saveObject)) {
        QuasarAppUtils::Params::verboseLog("AsyncSqlDbWriter: save object fail!",
                                           QuasarAppUtils::Error);
    }
}

void AsyncSqlDbWriter::handleDeleteObject(QSharedPointer<DBObject> deleteObject) {
    if (!SqlDBWriter::deleteObject(deleteObject)) {
        QuasarAppUtils::Params::verboseLog("AsyncSqlDbWriter: delete object fail!",
                                           QuasarAppUtils::Error);
    }
}

}
