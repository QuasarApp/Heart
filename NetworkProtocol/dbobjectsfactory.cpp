#include "dbobjectsfactory.h"

#include <dbobject.h>

namespace NetworkProtocol {

DbObjectsFactory::DbObjectsFactory() {

}

QSharedPointer<DBObject> DbObjectsFactory::factory(const QString & table) {
    return instance()->_data.value(table)->factory();
}

DbObjectsFactory *DbObjectsFactory::instance() {
    DbObjectsFactory * i = new DbObjectsFactory();
    return i;
}

bool DbObjectsFactory::registerObject(const QString & key, QSharedPointer<DBObject> obj) {
    if (obj.isNull())
        return false;

    _data[key] = obj;
    return true;
}

}
