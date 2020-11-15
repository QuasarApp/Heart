/*
 * Copyright (C) 2018-2020 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
*/


#ifndef TEMPLATEDBTESTS_H
#define TEMPLATEDBTESTS_H

#include <QFileInfo>
#include <databasenode.h>
#include <dbobjectsrequest.h>
#include <networkmember.h>
#include "test.h"
#include "sqldbcache.h"

template <class BASE, class WorkType>
/**
 * @brief The TemplateDataBaseNodeUnitTests class This is base class for testing databases.
 * You need to override this class for using. The BASE Templte is a any child class of the DataBaseNode class.
 */
class TemplateDataBaseNodeUnitTests: public BASE
{
public:
    TemplateDataBaseNodeUnitTests(const QString DbNodeName):BASE(){

        _dbNodeName = DbNodeName;

    }
    /**
     * @brief test - test work database
     * @return true if database of node work is correctly.
     */
    virtual bool test() {

        testObjec = randomMember();

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

protected:
    /**
     * @brief randomMember This method generate randm network member.
     * @return pointer to random network member
     */
    virtual const WorkType* randomMember() const = 0;

    /**
     * @brief init - init database.
     * @return return true if test module initialized successful
     */
    virtual bool init() {
        if (!BASE::run(TEST_LOCAL_HOST, TEST_PORT, _dbNodeName)) {
            return false;
        }

        QString database = BASE::dbLocation();
        BASE::stop();

        if (QFileInfo(database).exists() && !QFile::remove(database)) {
            return false;
        }

        return true;
    }

    /**
     * @brief testReadWrite - test save and get object functions
     * @return true if all test finished successful
     */
    bool testReadWrite() {
        if (!BASE::run(TEST_LOCAL_HOST, TEST_PORT, _dbNodeName)) {
            return false;
        }

        auto objectFromDataBase = BASE::db()->getObject(*testObjec);

        if (objectFromDataBase) {
            return false;
        }

        for (int i = 0; i < 10; ++i) {
            if (!BASE::db()->insertObject(randomMember())) {
                return false;
            }
        }

        QH::PKG::DBObjectsRequest<WorkType> setRequest(
                    testObjec->tableName(), "");

        auto list = BASE::db()->getObject(setRequest);

        if (list->data().size() != 10)
            return false;


        if (!BASE::db()->insertObject(testObjec)) {
            return false;
        }

        auto object = BASE::db()->getObject(*testObjec);

        if (!object || !object->isValid()) {
            return false;
        }

        BASE::stop();

        if (!BASE::run(TEST_LOCAL_HOST, TEST_PORT, _dbNodeName)) {
            return false;
        }

        objectFromDataBase = BASE::db()->getObject(*testObjec);

        if (objectFromDataBase && objectFromDataBase->trust() == 0) {
            return true;
        }

        return false;
    }

    /**
     * @brief testUpdate - test update functions
     * @return true if all test finished successful.
     */
    bool testUpdate() {
        BASE::stop();

        if (!BASE::run(TEST_LOCAL_HOST, TEST_PORT, _dbNodeName)) {
            return false;
        }


        auto objectFromDataBase = BASE::db()->getObject(*testObjec);

        if (!objectFromDataBase || objectFromDataBase->trust() != 0) {
            return false;
        }

        auto clone = objectFromDataBase->clone().template staticCast<QH::PKG::NetworkMember>();

        clone->setTrust(20);

        if (!BASE::db()->updateObject(clone.data())) {
            return false;
        }

        objectFromDataBase = BASE::db()->getObject(*testObjec);

        if (objectFromDataBase && objectFromDataBase->trust() != 20) {
            return false;
        }

        BASE::stop();

        if (!BASE::run(TEST_LOCAL_HOST, TEST_PORT, _dbNodeName)) {
            return false;
        }

        objectFromDataBase = BASE::db()->getObject(*testObjec);

        if (!objectFromDataBase || objectFromDataBase->trust() != 20) {
            return false;
        }

        return true;
    }

    /**
     * @brief testChangeTrust - this test check bad request responce for node
     * @return true if all test finished successful
     */
    bool testChangeTrust() {
        BASE::stop();

        if (!BASE::run(TEST_LOCAL_HOST, TEST_PORT, _dbNodeName)) {
            return false;
        }

        if(!BASE::changeTrust(testObjec->getId(), -10)) {
            return false;
        };

        auto objectFromDataBase = BASE::db()->getObject(*testObjec);

        if (objectFromDataBase && objectFromDataBase->trust() != 10) {
            return false;
        }

        BASE::stop();

        if (!BASE::run(TEST_LOCAL_HOST, TEST_PORT, _dbNodeName)) {
            return false;
        }

        objectFromDataBase = BASE::db()->getObject(*testObjec);

        if (!objectFromDataBase || objectFromDataBase->trust() != 10) {
            return false;
        }

        if(!BASE::changeTrust(testObjec->getId(), -10)) {
            return false;
        };

        objectFromDataBase = BASE::db()->getObject(*testObjec);

        if (objectFromDataBase && objectFromDataBase->trust() != 0) {
            return false;
        }

        BASE::stop();

        if (!BASE::run(TEST_LOCAL_HOST, TEST_PORT, _dbNodeName)) {
            return false;
        }

        objectFromDataBase = BASE::db()->getObject(*testObjec);

        if (!objectFromDataBase || objectFromDataBase->trust() != 0) {
            return false;
        }

        return BASE::isBanned(testObjec->getId());

    }

private:
    const WorkType *testObjec = nullptr;
    QString _dbNodeName = "DatabaseTestNode";

};

#endif // TEMPLATEDBTESTS_H
