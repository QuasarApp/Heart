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

bool BaseNodeInfo::isValid() const {
    return AbstractNodeInfo::isValid();
}

BaseNodeInfo::BaseNodeInfo(QAbstractSocket *tcp):
    AbstractNodeInfo(tcp){}

BaseNodeInfo::~BaseNodeInfo(){}

int BaseNodeInfo::permision(const QString &table, int id) const {
    auto allTablePermision = _permision.value(table).value(-1);
    auto itemPermision = _permision.value(table).value(id);

    return std::max(static_cast<int>(allTablePermision),
                    static_cast<int>(itemPermision));
}

int BaseNodeInfo::permision(const DbAddress &address) const {
    return permision(address.table, address.id);
}

void BaseNodeInfo::setPermision(const QString &table, int id, const Permission &permision) {
    _permision[table][id] = permision;
}

}
