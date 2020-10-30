#include "deleteobject.h"
namespace QH {
namespace PKG {
DeleteObject::DeleteObject(): DBObject("")  {

}

DeleteObject::DeleteObject(const Package &pkg): DeleteObject() {
    fromBytes(pkg.toBytes());
}

DBObject *DeleteObject::createDBObject() const {
    return create<DeleteObject>();
}

BaseId DeleteObject::generateId() const {
    return getId();
}

DBVariantMap DeleteObject::variantMap() const {
    return {};
}

bool DeleteObject::isCached() const {
    return false;
}
}
}
