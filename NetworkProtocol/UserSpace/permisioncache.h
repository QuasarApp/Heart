/*
 * Copyright (C) 2018-2020 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
*/

#ifndef PERMISIONCACHE_H
#define PERMISIONCACHE_H
#include "sqldbcache.h"
#include "userpermision.h"

namespace NP {

class UserNodeInfo;

/**
 * @brief The PermisionCache class - this implementation work with user permision table for check users permisions.
 *
 */
class NETWORKPROTOCOLSHARED_EXPORT PermisionCache: public SqlDBCache
{
    Q_OBJECT
public:
    PermisionCache();

    bool checkPermision(const NP::UserNodeInfo &requestNode,
                        const DBObject &saveObject,
                        Permission requiredPermision);

    bool saveObject(const DBObject *saveObject) override;

    // SqlDBCache interface
protected:
    void deleteFromCache(const DBObject* delObj) override;
    void saveToCache(const DBObject*obj) override;
    bool getFromCache(DBObject* obj) override;

private:
    QHash<PermisionData, Permission> _permisions;
    QSet<PermisionData> _needToSave;
    QMutex _savePermisionLaterMutex;


    // SqlDBCache interface
    void globalUpdateDataBasePrivate(qint64 currentTime) override;

};
}
#endif // PERMISIONCACHE_H
