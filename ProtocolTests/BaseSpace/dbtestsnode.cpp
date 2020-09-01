#include "dbtestsnode.h"
#include "test.h"
#include "sqldbcache.h"

#include <QFile>

DbTestsNode::DbTestsNode():NP::BaseNode() {

}

bool DbTestsNode::test() {
    QString database = "~/.local/QuasarApp/TestQNP/DatabaseTestNode/DatabaseTestNode_Storage.sqlite";
    if (QFile::exists(database) && !QFile::remove(database)) {
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

    return true;
}
