#include "basenodeunittests.h"
#include "test.h"
#include "sqldbcache.h"
#include <QFileInfo>
#include <QFile>
#include <networkmember.h>

#define DB_NODE_NAME "DatabaseTestNode"

QByteArray randomArray(int length) {
    char *buf = static_cast<char*>(malloc(length));
    QByteArray data;
    data.insert(0, buf, length);

    free(buf);

    return data;
}

const QH::NetworkMember* randomMember() {
    QH::NetworkMember *res = new QH::NetworkMember();

    res->setAuthenticationData(randomArray(64));
    res->setTrust(0);

    res->prepareToSend();

    return res;
}

BaseNodeUnitTests::BaseNodeUnitTests():QH::DataBaseNode(),
    testObjec(randomMember()) {

}

bool BaseNodeUnitTests::test() {

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

bool BaseNodeUnitTests::init() {
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

bool BaseNodeUnitTests::testReadWrite() {
    if (!run(TEST_LOCAL_HOST, TEST_PORT, DB_NODE_NAME)) {
        return false;
    }


    auto objectFromDataBase = db()->getObject(*testObjec);

    if (objectFromDataBase) {
        return false;
    }

    if (!db()->saveObject(testObjec)) {
        return false;
    }

    auto object = db()->getObject(*testObjec);

    if (!object || !object->isValid()) {
        return false;
    }

    stop();

    if (!run(TEST_LOCAL_HOST, TEST_PORT, DB_NODE_NAME)) {
        return false;
    }

    objectFromDataBase = db()->getObject(*testObjec);

    if (objectFromDataBase && objectFromDataBase->trust() == 0) {
        return true;
    }

    return false;
}

bool BaseNodeUnitTests::testUpdate() {
    stop();

    if (!run(TEST_LOCAL_HOST, TEST_PORT, DB_NODE_NAME)) {
        return false;
    }


    auto objectFromDataBase = db()->getObject(*testObjec);

    if (!objectFromDataBase || objectFromDataBase->trust() != 0) {
        return false;
    }

    auto clone = objectFromDataBase->clone().staticCast<QH::NetworkMember>();

    clone->setTrust(20);

    if (!db()->saveObject(clone.data())) {
        return false;
    }

    objectFromDataBase = db()->getObject(*testObjec);

    if (objectFromDataBase && objectFromDataBase->trust() != 20) {
        return false;
    }

    stop();

    if (!run(TEST_LOCAL_HOST, TEST_PORT, DB_NODE_NAME)) {
        return false;
    }

    objectFromDataBase = db()->getObject(*testObjec);

    if (!objectFromDataBase || objectFromDataBase->trust() != 20) {
        return false;
    }

    return true;
}

bool BaseNodeUnitTests::testChangeTrust() {
    stop();

    if (!run(TEST_LOCAL_HOST, TEST_PORT, DB_NODE_NAME)) {
        return false;
    }

    if(!changeTrust(testObjec->getId(), -10)) {
        return false;
    };

    auto objectFromDataBase = db()->getObject(*testObjec);

    if (objectFromDataBase && objectFromDataBase->trust() != 10) {
        return false;
    }

    stop();

    if (!run(TEST_LOCAL_HOST, TEST_PORT, DB_NODE_NAME)) {
        return false;
    }

    objectFromDataBase = db()->getObject(*testObjec);

    if (!objectFromDataBase || objectFromDataBase->trust() != 10) {
        return false;
    }

    if(!changeTrust(testObjec->getId(), -10)) {
        return false;
    };

    objectFromDataBase = db()->getObject(*testObjec);

    if (objectFromDataBase && objectFromDataBase->trust() != 0) {
        return false;
    }

    stop();

    if (!run(TEST_LOCAL_HOST, TEST_PORT, DB_NODE_NAME)) {
        return false;
    }

    objectFromDataBase = db()->getObject(*testObjec);

    if (!objectFromDataBase || objectFromDataBase->trust() != 0) {
        return false;
    }

    return isBanned(testObjec->getId());

}