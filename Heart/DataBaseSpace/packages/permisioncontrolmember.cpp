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

PermisionControlMember::PermisionControlMember(const QVariant &id): NetworkMember(id) {

}

DBObject *PermisionControlMember::createDBObject() const {
    return create<PermisionControlMember>();
}
}

}
