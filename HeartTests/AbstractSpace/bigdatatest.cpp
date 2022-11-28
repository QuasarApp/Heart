/*
 * Copyright (C) 2018-2022 QuasarApp.
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
        stream >> data;

        return stream;
    };

    QDataStream &toStream(QDataStream &stream) const override{
        stream << data;

        return stream;
    };
};


class BigDataTestParser: public QH::iParser {
    BigDataTestParser(QH::AbstractNode* parentNode): QH::iParser(parentNode) {
        registerPackageType<BigPackage>();
        data = new BigPackage();

    }

    // iParser interface
public:
    bool sendRequest(const QByteArray& data) {
        BigPackage pkg;
        pkg.data = data;

        return sendData(&pkg, QH::HostAddress(TEST_LOCAL_HOST, LOCAL_TEST_PORT));
    }

    const BigPackage* getData() const {
        QMutexLocker locker(&_mData);
        return data;
    }

    QH::ParserResult parsePackage(const QSharedPointer<QH::PKG::AbstractData> &pkg,
                                  const QH::Header &,
                                  QH::AbstractNodeInfo *sender) override {

        if (pkg->cmd() == BigPackage::command()) {

            _mData.lock();
            data->copy<BigPackage>(*pkg);
            _mData.unlock();
            sendData(data, sender);
            return QH::ParserResult::Processed;
        }

        return QH::ParserResult::NotProcessed;
    };
    int version() const override {return 0;};
    QString parserId() const override {return "BigDataTestParser";};

private:
    mutable QMutex _mData;
    BigPackage *data = nullptr;
};

class TestingClientBigData: public QH::AbstractNode {

    // AbstractNode interface
public:
    TestingClientBigData() {
        _parser = addApiParser<BigDataTestParser>();
    }

    const QSharedPointer<QH::iParser>& parser() const {
        return _parser;
    }

private:
    QSharedPointer<QH::iParser> _parser;
};

BigDataTest::BigDataTest() {
    _nodeA = new TestingClientBigData();
    _nodeAParser = static_cast<TestingClientBigData*>(_nodeA)->parser();

    _nodeB = new TestingClientBigData();
    _nodeBParser = static_cast<TestingClientBigData*>(_nodeB)->parser();

}

BigDataTest::~BigDataTest() {
    _nodeA->softDelete();
    _nodeB->softDelete();
}

void BigDataTest::test() {
//#ifdef Q_OS_LINUX

    QVERIFY(connectTest());
    QVERIFY(sendDataTest());
//#endif
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
        return _nodeBParser.staticCast<BigDataTestParser>()->sendRequest(testData);
    };

    auto client = _nodeBParser.staticCast<BigDataTestParser>();
    auto server = _nodeAParser.staticCast<BigDataTestParser>();

    auto check = [client, server, testData](){
        return client->getData() && server->getData()->data == testData;
    };

    return funcPrivateConnect(request, check);
}


