/*
 * Copyright (C) 2018-2021 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
*/

#ifndef SQLDB_H
#define SQLDB_H
#include "isqldbcache.h"

namespace QH {

/**
 * @brief The SqlDB class This is base implementation fo datatbase.
 * The SqlDB do not use caches, all request run on the sqlwtiter class.
 */
class HEARTSHARED_EXPORT SqlDB : public ISqlDBCache {
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
};
}
#endif // SQLDB_H
