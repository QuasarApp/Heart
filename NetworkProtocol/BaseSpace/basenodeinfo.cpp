/*
 * Copyright (C) 2018-2020 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
*/

#include "basenodeinfo.h"
#include "permisions.h"
#include "dbaddress.h"
#include <QTcpSocket>
#include <hostaddress.h>

namespace NP {


BaseNodeInfo::BaseNodeInfo(QAbstractSocket *tcp, const HostAddress* address):
    AbstractNodeInfo(tcp, address){}

BaseNodeInfo::~BaseNodeInfo() = default;

bool BaseNodeInfo::isValid() const {
    return AbstractNodeInfo::isValid();
}

bool BaseNodeInfo::isKnowAddress(const BaseId &address) const {
    return _knowAddresses.contains(address);
}

BaseId BaseNodeInfo::selfId() const {
    return _selfId;
}

void BaseNodeInfo::setSelfId(const BaseId &selfId) {
    _selfId = selfId;
    _knowAddresses.insert(_selfId);
}

void BaseNodeInfo::addKnowAddresses(const QSet<BaseId> &newAddressses) {
    _knowAddresses += newAddressses;
}

bool BaseNodeInfo::confirmData() const {
    return AbstractNodeInfo::confirmData() && _selfId.isValid();
}

}
