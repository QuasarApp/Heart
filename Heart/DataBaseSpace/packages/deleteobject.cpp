/*
 * Copyright (C) 2018-2020 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
*/

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
