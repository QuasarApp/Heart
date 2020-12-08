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
    QByteArray data;

    static unsigned char nance = 0;

    for (int i = 0 ; i < length; ++i) {
        data.push_back((rand() % static_cast<unsigned char>(0xFF - nance)) + nance);
    }

    nance++;

    return data;
}

DataBaseNodeUnitTests::DataBaseNodeUnitTests():
    TemplateDataBaseNodeUnitTests<QH::DataBaseNode, QH::PKG::UserMember>(DB_NODE_NAME)
{

}

QH::PKG::UserMember *DataBaseNodeUnitTests::randomMember() const {
    srand(time(nullptr));

    QH::PKG::UserMember * res = new QH::PKG::UserMember();
    res->setAuthenticationData(randomArray(64));
    res->setTrust(0);
    res->setName(randomArray(5).toHex());

    res->prepareToSend();

    return res;
}

