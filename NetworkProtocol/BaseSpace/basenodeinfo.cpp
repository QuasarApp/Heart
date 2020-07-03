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
#include <QHostAddress>

namespace NP {


BaseNodeInfo::BaseNodeInfo(QAbstractSocket *tcp):
    AbstractNodeInfo(tcp){}

BaseNodeInfo::~BaseNodeInfo() = default;


bool BaseNodeInfo::isValid() const {
    return AbstractNodeInfo::isValid();
}

bool BaseNodeInfo::isKnowAddress(const BaseId &address) const {
    return _knowAddresses.contains(address);
}

}
