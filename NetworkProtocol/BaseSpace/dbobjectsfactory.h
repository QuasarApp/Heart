/*
 * Copyright (C) 2018-2020 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
*/

#ifndef DBOBJECTSFACTORY_H
#define DBOBJECTSFACTORY_H
#include "networkprotocol_global.h"

#include <QSharedPointer>

namespace NP {

class DBObject;

class NETWORKPROTOCOLSHARED_EXPORT DbObjectsFactory
{
public:
    static SP<DBObject> factory(const QString&);
    static DbObjectsFactory* instance();
    bool registerObject(const QString&key, SP<DBObject> obj);

private:
    DbObjectsFactory();

    QHash<QString, SP<DBObject>> _data;

};
}

#endif // DBOBJECTSFACTORY_H
