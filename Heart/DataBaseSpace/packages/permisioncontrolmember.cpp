/*
 * Copyright (C) 2018-2020 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
*/

#include "permisioncontrolmember.h"

namespace QH {
namespace PKG {

PermisionControlMember::PermisionControlMember() {

}

PermisionControlMember::PermisionControlMember(const BaseId &id): NetworkMember(id) {

}

bool PermisionControlMember::isCached() const {
    return false;
}

BaseId PermisionControlMember::generateId() const {
    return getId();
}

DBObject *PermisionControlMember::createDBObject() const {
    return create<PermisionControlMember>();
}
}

}
