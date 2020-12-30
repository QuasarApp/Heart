/*
 * Copyright (C) 2018-2020 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
*/


#ifndef TEMPLATEDBTESTS_H
#define TEMPLATEDBTESTS_H

#include <QFileInfo>
#include <cacheddbobjectsrequest.h>
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

        testObjec = QSharedPointer<WorkType>(randomMember());
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

        BASE::stop();
        return true;
    }

protected:
    /**
     * @brief randomMember This method generate randm network member.
     * @return pointer to random network member
     */
    virtual WorkType* randomMember() const = 0;

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
        // test init database
        if (!BASE::run(TEST_LOCAL_HOST, TEST_PORT, _dbNodeName)) {
            return false;
        }

        // try get not exists object
        auto objectFromDataBase = BASE::db()->getObject(*testObjec);

        // objectFromDataBase must be equal 0x0
        if (objectFromDataBase) {
            return false;
        }

        // generate random objects for database.
        for (int i = 0; i < 100; ++i) {
            if (!BASE::db()->insertObject(QSharedPointer<WorkType>(randomMember()), true)) {
                return false;
            }
        }

        // create request for get all objects from database.
        QH::PKG::DBObjectsRequest<WorkType> setRequest(
                    testObjec->tableName(), "");

        auto list = BASE::db()->getObject(setRequest);

        if (list && list->data().size() != 100)
            return false;

        // create request for get all objects from database and cached it.
        QH::PKG::CachedDbObjectsRequest<WorkType> request("");

        QList<QSharedPointer<QH::PKG::DBObject>> result;
        if (!BASE::db()->getAllObjects(request, result)) {
            return false;
        }

        if (list->data().size() != result.size())
            return false;

        // insert main object inot database.
        if (!BASE::db()->insertObject(testObjec)) {
            return false;
        }

        // get main object from cache database.
        auto object = BASE::db()->getObject(*testObjec);

        if (!object || !object->isValid()) {
            return false;
        }

        testObjec->copyFrom(object.data());

        // save state of cache of data base.
        BASE::stop();

        // run new session of server dataqbase.
        if (!BASE::run(TEST_LOCAL_HOST, TEST_PORT, _dbNodeName)) {
            return false;
        }

        // try get object from not cache.
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

        if (!BASE::db()->updateObject(clone)) {
            return false;
        }

        objectFromDataBase = BASE::db()->getObject(*objectFromDataBase);

        if (objectFromDataBase && objectFromDataBase->trust() != 20) {
            return false;
        }

        BASE::stop();

        if (!BASE::run(TEST_LOCAL_HOST, TEST_PORT, _dbNodeName)) {
            return false;
        }

        objectFromDataBase = BASE::db()->getObject(*objectFromDataBase);

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

        if(BASE::changeTrust(QVariant{}, -10)) {
            return false;
        };

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

    QByteArray randomArray(int length) const {
        QByteArray data;

        static unsigned char nance = 0;

        for (int i = 0 ; i < length; ++i) {
            data.push_back(((rand() + nance) % static_cast<unsigned char>(0xFF)));
        }

        nance++;

        return data;
    }

private:
    QSharedPointer<WorkType> testObjec = nullptr;
    QString _dbNodeName = "DatabaseTestNode";

};

#endif // TEMPLATEDBTESTS_H
