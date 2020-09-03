#include "dbtestsnode.h"
#include "test.h"
#include "sqldbcache.h"
#include <QFileInfo>
#include <QFile>

DbTestsNode::DbTestsNode():NP::BaseNode() {

}

bool DbTestsNode::test() {


    if (!run(TEST_LOCAL_HOST, TEST_PORT, "DatabaseTestNode")) {
        return false;
    }

    QString database = dbLocation();
    stop();


    if (QFileInfo(database).exists() && !QFile::remove(database)) {
        return false;
    }

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
