/*
 * Copyright (C) 2018-2020 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
*/

#include "networknodeinfo.h"
#include "dbaddress.h"
#include <QTcpSocket>
#include <hostaddress.h>

namespace NP {


NetworkNodeInfo::NetworkNodeInfo(QAbstractSocket *tcp, const HostAddress* address):
    BaseNodeInfo(tcp, address){}

NetworkNodeInfo::~NetworkNodeInfo() = default;

bool NetworkNodeInfo::isValid() const {
    return BaseNodeInfo::isValid();
}

bool NetworkNodeInfo::isKnowAddress(const BaseId &address) const {
    return _knowAddresses.contains(address);
}

void NetworkNodeInfo::setSelfId(const BaseId &selfId) {
    BaseNodeInfo::setSelfId(selfId);
    _knowAddresses.insert(_selfId);
}

void NetworkNodeInfo::addKnowAddresses(const QSet<BaseId> &newAddressses) {
    _knowAddresses += newAddressses;
}

bool NetworkNodeInfo::confirmData() const {
    return BaseNodeInfo::confirmData() && _selfId.isValid();
}

}
