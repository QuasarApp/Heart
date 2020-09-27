/*
 * Copyright (C) 2018-2020 QuasarApp.
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

NetworkNodeUnitTests::NetworkNodeUnitTests():QH::NetworkNode() {

}

bool NetworkNodeUnitTests::test() {

    if (!init()) {
        return false;
    }

    if (!testReadWrite()) {
        return false;
    }

    if (!testUpdate()) {
        return false;
    }

    if (!testChangeTrust()) {
        return false;
    }

    return true;
}

bool NetworkNodeUnitTests::init() {
    if (!run(TEST_LOCAL_HOST, TEST_PORT, DB_NODE_NAME)) {
        return false;
    }

    QString database = dbLocation();
    stop();


    if (QFileInfo(database).exists() && !QFile::remove(database)) {
        return false;
    }

    return true;
}

bool NetworkNodeUnitTests::testReadWrite() {
    if (!run(TEST_LOCAL_HOST, TEST_PORT, DB_NODE_NAME)) {
        return false;
    }

    QH::PKG::NodeObject testObjec = thisNode();

    auto objectFromDataBase = db()->getObject(testObjec);

    if (objectFromDataBase) {
        return false;
    }

    if (!db()->saveObject(&testObjec)) {
        return false;
    }

    auto object = db()->getObject(testObjec);

    if (!object || !object->isValid()) {
        return false;
    }

    stop();

    if (!run(TEST_LOCAL_HOST, TEST_PORT, DB_NODE_NAME)) {
        return false;
    }

    objectFromDataBase = db()->getObject(testObjec);

    if (objectFromDataBase && objectFromDataBase->trust() == 0) {
        return true;
    }

    return false;
}

bool NetworkNodeUnitTests::testUpdate() {
    stop();

    if (!run(TEST_LOCAL_HOST, TEST_PORT, DB_NODE_NAME)) {
        return false;
    }

    QH::PKG::NodeObject testObjec = thisNode();

    auto objectFromDataBase = db()->getObject(testObjec);

    if (!objectFromDataBase || objectFromDataBase->trust() != 0) {
        return false;
    }

    auto clone = objectFromDataBase->clone().staticCast<QH::PKG::NodeObject>();

    clone->setTrust(20);

    if (!db()->saveObject(clone.data())) {
        return false;
    }

    objectFromDataBase = db()->getObject(testObjec);

    if (objectFromDataBase && objectFromDataBase->trust() != 20) {
        return false;
    }

    stop();

    if (!run(TEST_LOCAL_HOST, TEST_PORT, DB_NODE_NAME)) {
        return false;
    }

    objectFromDataBase = db()->getObject(testObjec);

    if (!objectFromDataBase || objectFromDataBase->trust() != 20) {
        return false;
    }

    return true;
}

bool NetworkNodeUnitTests::testChangeTrust() {
    stop();

    if (!run(TEST_LOCAL_HOST, TEST_PORT, DB_NODE_NAME)) {
        return false;
    }

    QH::PKG::NodeObject testObjec = thisNode();

    if(!changeTrust(testObjec.getId(), -10)) {
        return false;
    };

    auto objectFromDataBase = static_cast<const QH::PKG::NodeObject*>(db()->getObjectRaw(testObjec));

    if (objectFromDataBase && objectFromDataBase->trust() != 10) {
        return false;
    }

    stop();

    if (!run(TEST_LOCAL_HOST, TEST_PORT, DB_NODE_NAME)) {
        return false;
    }

    objectFromDataBase = static_cast<const QH::PKG::NodeObject*>(db()->getObjectRaw(testObjec));

    if (!objectFromDataBase || objectFromDataBase->trust() != 10) {
        return false;
    }

    if(!changeTrust(testObjec.getId(), -10)) {
        return false;
    };

    objectFromDataBase = static_cast<const QH::PKG::NodeObject*>(db()->getObjectRaw(testObjec));

    if (objectFromDataBase && objectFromDataBase->trust() != 0) {
        return false;
    }

    stop();

    if (!run(TEST_LOCAL_HOST, TEST_PORT, DB_NODE_NAME)) {
        return false;
    }

    objectFromDataBase = static_cast<const QH::PKG::NodeObject*>(db()->getObjectRaw(testObjec));

    if (!objectFromDataBase || objectFromDataBase->trust() != 0) {
        return false;
    }

    return isBanned(testObjec.getId());

}
