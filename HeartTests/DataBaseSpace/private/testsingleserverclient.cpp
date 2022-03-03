/*
 * Copyright (C) 2020-2022 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
*/


#include "testsingleserverclient.h"
#include "test.h"

const QH::PKG::Ping &TestSingleServerClient::getPing() const {
    return _ping;
}

void TestSingleServerClient::setStatus(const QH::ClientStatus &status) {
    QH::SingleClient::setStatus(status);
}

QPair<QString, unsigned short> TestSingleServerClient::serverAddress() const {
    return {TEST_LOCAL_HOST, LOCAL_TEST_PORT};
}

void TestSingleServerClient::incomingData(const QH::PKG::AbstractData *pkg,
                                          const QH::AbstractNodeInfo *sender) {
    Q_UNUSED(sender)

    auto ping = dynamic_cast<const QH::PKG::Ping*>(pkg);
    if (ping)
        _ping.setAnsver(ping->ansver());
}
