/*
 * Copyright (C) 2018-2021 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
*/

#include "bigdatatest.h"

#include <abstractnode.h>

#define LOCAL_TEST_PORT TEST_PORT + 4

class BigPackage: public QH::PKG::AbstractData {
    QH_PACKAGE(BigPackage, "BigPackage")
public:
    BigPackage(){
        data = {};
    };
    QByteArray data;

    // StreamBase interface
protected:
    QDataStream &fromStream(QDataStream &stream) override{
        AbstractData::fromStream(stream);

        stream >> data;

        return stream;
    };

    QDataStream &toStream(QDataStream &stream) const override{
        AbstractData::toStream(stream);

        stream << data;

        return stream;
    };
};

class TestingClientBigData: public QH::AbstractNode {


    // AbstractNode interface
public:
    TestingClientBigData(int i) {
        Q_UNUSED(i);
        registerPackageType<BigPackage>();
        data = new BigPackage();
    }

    bool sendRequest(const QByteArray& data) {
        BigPackage pkg;
        pkg.data = data;

        return sendData(&pkg, QH::HostAddress(TEST_LOCAL_HOST, LOCAL_TEST_PORT));
    }

    const BigPackage* getData() const {
        return data;
    }

protected:
    void incomingData(const QH::PKG::AbstractData *pkg, const QH::AbstractNodeInfo *sender) override {
        Q_UNUSED(sender);

        if (pkg->cmd() == BigPackage::command()) {

            data->copy<BigPackage>(*pkg);
            sendData(data, sender);
        }
    }

private:
    BigPackage *data = nullptr;
};

BigDataTest::BigDataTest() {
    _nodeA = new TestingClientBigData(0);
    _nodeB = new TestingClientBigData(0);
}

BigDataTest::~BigDataTest() {
    _nodeA->softDelete();
    _nodeB->softDelete();
}

void BigDataTest::test() {
#ifdef Q_OS_LINUX

    QVERIFY(connectTest());
    QVERIFY(sendDataTest());
#endif
}

bool BigDataTest::connectTest() {

    if (!_nodeA->run(TEST_LOCAL_HOST, LOCAL_TEST_PORT)) {
        return false;
    }

    return connectFunc(_nodeB, TEST_LOCAL_HOST, LOCAL_TEST_PORT);
}

bool BigDataTest::sendDataTest() {


    QByteArray testData;
    for (int i = 0; i < 1024 * 1024 * 10; i++) {
        testData.push_back(rand());
    }

    auto request = [this, testData](){
        return static_cast<TestingClientBigData*>(_nodeB)->sendRequest(testData);
    };

    auto client = dynamic_cast<TestingClientBigData*>(_nodeB);

    auto check = [client, testData](){
        return client->getData() && client->getData()->data == testData;
    };

    return funcPrivateConnect(request, check);
}
