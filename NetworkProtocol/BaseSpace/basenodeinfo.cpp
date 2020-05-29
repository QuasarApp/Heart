/*
 * Copyright (C) 2018-2020 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
*/

#include "basenodeinfo.h"
#include <QTcpSocket>
#include <QHostAddress>

namespace NP {

bool BaseNodeInfo::isValid() const {
    return AbstractNodeInfo::isValid();
}

BaseNodeInfo::BaseNodeInfo(QAbstractSocket *tcp):
    AbstractNodeInfo(tcp){}

BaseNodeInfo::~BaseNodeInfo(){}

const AccessToken &BaseNodeInfo::token() const {
    return _token;
}

void BaseNodeInfo::setToken(const AccessToken &token) {
    _token = token;
}

Permission BaseNodeInfo::permision(const QString &table, int id) const {
    auto allTablePermision = _permision.value(table).value(-1);
    auto itemPermision = _permision.value(table).value(id);

    return std::max(allTablePermision, itemPermision);
}

void BaseNodeInfo::setPermision(const QString &table, int id, const Permission &permision) {
    _permision[table][id] = permision;
}

}
