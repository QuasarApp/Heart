/*
 * Copyright (C) 2018-2021 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
*/

#include "networkmember.h"

namespace QH {
namespace PKG {

NetworkMember::NetworkMember() {

}

NetworkMember::NetworkMember(const QVariant &id): AbstractNetworkMember(id) {

}

DBObject *NetworkMember::createDBObject() const {
    return create<NetworkMember>();
}
}

}
