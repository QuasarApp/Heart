#include "asyncsqldbwriter.h"

#include <QMetaMethod>
#include <QThread>
#include <quasarapp.h>

namespace ClientProtocol {


AsyncSqlDbWriter::AsyncSqlDbWriter(QObject *ptr):
    QObject(ptr)
{
    QThread *own = new QThread(this);
    moveToThread(own);
}

bool AsyncSqlDbWriter::saveObject(QWeakPointer<DBObject> saveObject) {
   return QMetaObject::invokeMethod(this,
                              "handleSaveObject",
                              Qt::QueuedConnection,
                              Q_ARG(QWeakPointer<DBObject>, saveObject));

}

bool AsyncSqlDbWriter::deleteObject(const QString &table, int id) {
    return QMetaObject::invokeMethod(this,
                               "handleSaveObject",
                               Qt::QueuedConnection,
                               Q_ARG(QString, table), Q_ARG(int, id));
}

void AsyncSqlDbWriter::handleSaveObject(QWeakPointer<DBObject> saveObject) {
    if (!SqlDBWriter::saveObject(saveObject)) {
        QuasarAppUtils::Params::verboseLog("AsyncSqlDbWriter: save object fail!",
                                           QuasarAppUtils::Error);
    }
}

void AsyncSqlDbWriter::handleDeleteObject(const QString &table, int id) {
    if (!SqlDBWriter::deleteObject(table, id)) {
        QuasarAppUtils::Params::verboseLog("AsyncSqlDbWriter: delete object fail!",
                                           QuasarAppUtils::Error);
    }
}

}
