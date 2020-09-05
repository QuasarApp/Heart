#include "basenodeunittests.h"
#include "test.h"
#include "sqldbcache.h"
#include <QFileInfo>
#include <QFile>

BaseNodeUnitTests::BaseNodeUnitTests():NP::BaseNode() {

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
    if (!run(TEST_LOCAL_HOST, TEST_PORT, "DatabaseTestNode")) {
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
    if (!run(TEST_LOCAL_HOST, TEST_PORT, "DatabaseTestNode")) {
        return false;
    }

    NP::NodeObject testObjec = thisNode();

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

    if (!run(TEST_LOCAL_HOST, TEST_PORT, "DatabaseTestNode")) {
        return false;
    }

    objectFromDataBase = db()->getObject(testObjec);

    if (objectFromDataBase && objectFromDataBase->trust() == 0) {
        return true;
    }

    return false;
}

bool BaseNodeUnitTests::testUpdate() {
    stop();

    if (!run(TEST_LOCAL_HOST, TEST_PORT, "DatabaseTestNode")) {
        return false;
    }

    NP::NodeObject testObjec = thisNode();

    auto objectFromDataBase = db()->getObject(testObjec);

    if (!objectFromDataBase || objectFromDataBase->trust() != 0) {
        return false;
    }

    auto clone = static_cast<NP::NodeObject*>(objectFromDataBase->clone());

    clone->setTrust(20);

    if (!db()->saveObject(objectFromDataBase)) {
        return false;
    }

    objectFromDataBase = db()->getObject(testObjec);

    if (objectFromDataBase && objectFromDataBase->trust() == 20) {
        return true;
    }

    return false;
}

bool BaseNodeUnitTests::testChangeTrust() {
    stop();

    if (!run(TEST_LOCAL_HOST, TEST_PORT, "DatabaseTestNode")) {
        return false;
    }

    NP::NodeObject testObjec = thisNode();

    return false;

}
