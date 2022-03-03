/*
 * Copyright (C) 2018-2022 QuasarApp.
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
#include <itest.h>
#include <test.h>
#include <abstractnetworkmember.h>
#include "sqldbcache.h"
#include <QtTest>


template <class BASE, class WorkType>
/**
 * @brief The TemplateDataBaseNodeUnitTests class This is base class for testing databases.
 * You need to override this class for using. The BASE Templte is a any child class of the DataBaseNode class.
 */
class TemplateDataBaseNodeUnitTests: public BASE, public iTest
{
public:
    TemplateDataBaseNodeUnitTests(const QString DbNodeName):BASE(){

        _dbNodeName = DbNodeName;

    }
    /**
     * @brief test - test work database
     * @return true if database of node work is correctly.
     */
    void test() override{

        testObjec = QSharedPointer<WorkType>(randomMember());

        initUnitTests();
        testReadWrite();
        testUpdate();
        testChangeTrust();

        BASE::stop();
    }

protected:
    /**
     * @brief randomMember This method generate randm network member.
     * @return pointer to random network member
     */
    virtual WorkType* randomMember() const = 0;

    /**
     * @brief initUnitTests - init database.
     * @return return true if test module initialized successful
     */
    virtual void initUnitTests() {
        QVERIFY(BASE::run(_dbNodeName));

        QString database = BASE::dbLocation();
        BASE::stop();

        QVERIFY(QFileInfo::exists(database));
        QVERIFY(QFile::remove(database));
    }

    /**
     * @brief testReadWrite - test save and get object functions
     * @return true if all test finished successful
     */
    void testReadWrite() {
        // test init database
        QVERIFY(BASE::run(_dbNodeName));


        // try get not exists object
        auto objectFromDataBase = BASE::db()->getObject(*testObjec);

        // objectFromDataBase must be equal 0x0
        QVERIFY(!objectFromDataBase);

        // generate random objects for database.
        for (int i = 0; i < 100; ++i) {
            auto obj = QSharedPointer<WorkType>(randomMember());

            QVERIFY(obj->isValid());

            QVERIFY(BASE::db()->insertObject(obj, true));
        }

        // create request for get all objects from database.
        QH::PKG::DBObjectsRequest<WorkType> setRequest(
                    testObjec->tableName(), "");

        auto list = BASE::db()->getObject(setRequest);
        QVERIFY(list);
        QVERIFY(list->data().size() == 100);

        // create request for get all objects from database and cached it.
        QH::PKG::CachedDbObjectsRequest<WorkType> request("");

        QList<QSharedPointer<QH::PKG::DBObject>> result;
        QVERIFY(BASE::db()->getAllObjects(request, result));

        QVERIFY(list->data().size() == result.size());

        // insert main object inot database.
        QVERIFY(BASE::db()->insertObject(testObjec));

        // get main object from cache database.
        auto object = BASE::db()->getObject(*testObjec);

        QVERIFY(object);
        QVERIFY(object->isValid());

        testObjec->copyFrom(object.data());

        // save state of cache of data base.
        BASE::stop();

        // run new session of server dataqbase.
        QVERIFY(BASE::run(_dbNodeName));

        // try get object from not cache.
        objectFromDataBase = BASE::db()->getObject(*testObjec);

        QVERIFY(objectFromDataBase);
        QVERIFY(objectFromDataBase->trust() == 0);

    }

    /**
     * @brief testUpdate - test update functions
     * @return true if all test finished successful.
     */
    void testUpdate() {
        BASE::stop();

        QVERIFY(BASE::run(_dbNodeName));


        auto objectFromDataBase = BASE::db()->getObject(*testObjec);

        QVERIFY(objectFromDataBase);
        QVERIFY(objectFromDataBase->trust() == 0);

        auto clone = objectFromDataBase->clone().template staticCast<QH::PKG::AbstractNetworkMember>();

        clone->setTrust(20);

        QVERIFY(BASE::db()->updateObject(clone));

        objectFromDataBase = BASE::db()->getObject(*objectFromDataBase);

        QVERIFY(objectFromDataBase);
        QVERIFY(objectFromDataBase->trust() == 20);

        BASE::stop();

        QVERIFY(BASE::run(_dbNodeName));

        objectFromDataBase = BASE::db()->getObject(*objectFromDataBase);

        QVERIFY(objectFromDataBase);
        QVERIFY(objectFromDataBase->trust() == 20);

    }

    /**
     * @brief testChangeTrust - this test check bad request responce for node
     * @return true if all test finished successful
     */
    void testChangeTrust() {
        BASE::stop();

        QVERIFY(BASE::run( _dbNodeName));

        QVERIFY(!BASE::changeTrust(QVariant{}, -10));

        QVERIFY(BASE::changeTrust(testObjec->getId(), -10));

        auto objectFromDataBase = BASE::db()->getObject(*testObjec);

        QVERIFY(objectFromDataBase);
        QVERIFY(objectFromDataBase->trust() == 10);

        BASE::stop();

        QVERIFY(BASE::run(_dbNodeName));

        objectFromDataBase = BASE::db()->getObject(*testObjec);

        QVERIFY(objectFromDataBase);
        QVERIFY(objectFromDataBase->trust() == 10);

        QVERIFY(BASE::changeTrust(testObjec->getId(), -10));

        objectFromDataBase = BASE::db()->getObject(*testObjec);

        QVERIFY(objectFromDataBase);
        QVERIFY(objectFromDataBase->trust() == 0);

        BASE::stop();

        QVERIFY(BASE::run(_dbNodeName));

        objectFromDataBase = BASE::db()->getObject(*testObjec);

        QVERIFY(objectFromDataBase);
        QVERIFY(objectFromDataBase->trust() == 0);

        QVERIFY(BASE::isBanned(testObjec->getId()));

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
    QSharedPointer<QH::PKG::AbstractNetworkMember> testObjec = nullptr;
    QString _dbNodeName = "DatabaseTestNode";
    int _testPort = TEST_PORT + 2;

};

#endif // TEMPLATEDBTESTS_H
