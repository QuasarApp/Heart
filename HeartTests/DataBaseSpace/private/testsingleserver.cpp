/*
 * Copyright (C) 2020-2022 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
*/

#include "testsingleserver.h"

const QH::PKG::Ping &TestSingleServer::getPing() const {
    return _ping;
}

void TestSingleServer::incomingData(const QH::PKG::AbstractData *pkg, const QH::AbstractNodeInfo *sender) {
    Q_UNUSED(sender)

    auto ping = dynamic_cast<const QH::PKG::Ping*>(pkg);
    if (ping)
        _ping.setAnsver(ping->ansver());
}
