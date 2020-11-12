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

namespace QH {


NetworkNodeInfo::NetworkNodeInfo(QAbstractSocket *tcp, const HostAddress* address):
    AbstractNodeInfo(tcp, address){}

NetworkNodeInfo::~NetworkNodeInfo() = default;

bool NetworkNodeInfo::isValid() const {
    return AbstractNodeInfo::isValid();
}

bool NetworkNodeInfo::isKnowAddress(const NodeId &address) const {
    return _knowAddresses.contains(address);
}

void NetworkNodeInfo::setSelfId(const NodeId &selfId) {
    _selfId = selfId;
    _knowAddresses.insert(_selfId);
}

void NetworkNodeInfo::addKnowAddresses(const QSet<NodeId> &newAddressses) {
    _knowAddresses += newAddressses;
}

bool NetworkNodeInfo::confirmData() const {
    return AbstractNodeInfo::confirmData() && _selfId.isValid();
}

const NodeId &NetworkNodeInfo::selfId() const
{
    return _selfId;
}

}
