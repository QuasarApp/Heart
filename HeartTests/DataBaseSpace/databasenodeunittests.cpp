/*
 * Copyright (C) 2018-2020 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
*/
#include "databasenodeunittests.h"

#include <usermember.h>

#define DB_NODE_NAME "DatabaseTestNode"

QByteArray randomArray(int length) {
    char *buf = static_cast<char*>(malloc(length));
    QByteArray data;
    data.insert(0, buf, length);

    free(buf);

    return data;
}

DataBaseNodeUnitTests::DataBaseNodeUnitTests():
    TemplateDataBaseNodeUnitTests<QH::DataBaseNode>(DB_NODE_NAME)
{

}

const QH::PKG::NetworkMember *DataBaseNodeUnitTests::randomMember() const {
    QH::PKG::NetworkMember * res = new QH::PKG::UserMember();
    res->setId(randomArray(10).toBase64());
    res->setAuthenticationData(randomArray(64));
    res->setTrust(0);

    res->prepareToSend();

    return res;
}
