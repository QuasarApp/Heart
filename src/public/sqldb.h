/*
 * Copyright (C) 2018-2025 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
*/

#ifndef SQLDB_H
#define SQLDB_H
#include "isqldb.h"

namespace QH {

/**
 * @brief The SqlDB class This is base implementation fo datatbase.
 * The SqlDB do not use caches, all request run on the sqlwtiter class.
 */
class HEARTSHARED_EXPORT SqlDB final: public ISqlDB {
    Q_OBJECT
public:
    SqlDB();

    // ISqlDBCache interface
protected:
    void deleteFromCache(const QSharedPointer<PKG::DBObject> &delObj) override final;
    bool insertToCache(const QSharedPointer<PKG::DBObject> &obj) override final;
    bool updateCache(const QSharedPointer<PKG::DBObject> &obj) override final;
    QList<QSharedPointer<PKG::DBObject>>
    getFromCache(const PKG::DBObject *obj) override final;
    void pushToQueue(const QSharedPointer<PKG::DBObject> &obj, CacheAction type) override final;
    void globalUpdateDataBasePrivate(qint64 currentTime) override final;
    void globalUpdateDataBase(SqlDBCasheWriteMode mode) override final;
};
}
#endif // SQLDB_H
