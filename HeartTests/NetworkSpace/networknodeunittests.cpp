/*
 * Copyright (C) 2018-2021 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
*/


#include "networknodeunittests.h"
#include "test.h"
#include "sqldbcache.h"
#include <QFileInfo>
#include <QFile>

#define DB_NODE_NAME "DbTestNetworkNode"

NetworkNodeUnitTests::NetworkNodeUnitTests():
    TemplateDataBaseNodeUnitTests<QH::NetworkNode, QH::PKG::NodeObject>(DB_NODE_NAME) {

}

QH::PKG::NodeObject *NetworkNodeUnitTests::randomMember() const {

    srand(time(nullptr));

    QH::PKG::NodeObject * res = new QH::PKG::NodeObject();

    res->setAuthenticationData(randomArray(64));
    res->setTrust(0);
    res->prepareToSend();

    return res;
}
