/*
 * Copyright (C) 2018-2025 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
*/


#include "testutils.h"

#include <QCoreApplication>
#include <QDateTime>
#include <QVariantMap>
#include <abstractnode.h>
#include <hostaddress.h>

bool TestUtils::funcPrivateConnect(const std::function<bool()> &requestFunc,
                                   const std::function<bool()> &checkFunc,
                                   const std::function<QMetaObject::Connection()> &connectFunction) const {

    QMetaObject::Connection m_connection = connectFunction();
    if (requestFunc && !requestFunc()) {
        QObject::disconnect(m_connection);
        return false;
    }

    bool return_value = TestUtils::wait(checkFunc, WAIT_RESPOCE_TIME);
    QObject::disconnect(m_connection);

    return return_value;
}

bool TestUtils::funcPrivateConnect(const std::function<bool ()> &requestFunc,
                                   const std::function<bool ()> &checkFunc) const {
    return funcPrivateConnect(requestFunc, checkFunc, [](){return QMetaObject::Connection();});
}

TestUtils::TestUtils()
{

}

TestUtils::~TestUtils() {

}

bool TestUtils::wait(const std::function<bool()> &forWait, int msec) const {
    auto curmsec = QDateTime::currentMSecsSinceEpoch() + msec;
    bool waitFor = false;
    while (curmsec > QDateTime::currentMSecsSinceEpoch() && !waitFor) {
        waitFor = forWait();
        QCoreApplication::processEvents();
    }
    QCoreApplication::processEvents();
    return waitFor;
}

bool TestUtils::connectFunc(
        QH::AbstractNode *cli,
        const QString& address,
        unsigned short port) const {

    auto wraper = [&cli, address, port]() {
        cli->addNode(QH::HostAddress{address, port});
        return true;
    };

    auto check = [&cli]() {
        return cli->confirmendCount();
    };

    return funcPrivateConnect(wraper, check, [](){ return QMetaObject::Connection{};});
}
