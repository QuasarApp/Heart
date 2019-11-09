#ifndef DBOBJECTSFACTORY_H
#define DBOBJECTSFACTORY_H
#include "networkprotocol_global.h"

#include <QSharedPointer>

namespace NetworkProtocol {

class DBObject;

class NETWORKPROTOCOLSHARED_EXPORT DbObjectsFactory
{
public:
    static QSharedPointer<DBObject> factory(const QString&);
    static DbObjectsFactory* instance();
    bool registerObject(const QString&key, QSharedPointer<DBObject> obj);

private:
    DbObjectsFactory();

    QHash<QString, QSharedPointer<DBObject>> _data;

};
}

#endif // DBOBJECTSFACTORY_H
