/*
 * Copyright (C) 2023-2024 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
*/

#include "multiversiontest.h"

#define LOCAL_TEST_PORT TEST_PORT + 6

class MultiVersionPkg {
public:
    int v2;
    int v1;
    bool responce = false;
    int lastSerializedFrom = 0;
    int lastSerializedto = 0;
};

class MultiVersionPkg1: public QH::PKG::MultiversionData, public MultiVersionPkg {
    QH_PACKAGE("MultiVersionPkg")

public:
    MultiVersionPkg1():  QH::PKG::MultiversionData(
            {
             {0, // version 0
                 {
                     [this](QDataStream& stream) -> QDataStream&{ // from
                         lastSerializedFrom = 0;
                         stream >> v1;
                         stream >> responce;

                         return stream;
                     },
                     [this](QDataStream& stream) -> QDataStream&{ // to
                         lastSerializedto = 0;

                         stream << v1;
                         stream << responce;

                         return stream;
                     }
                 }
             },
             {1, // version 1
                 {
                     [this](QDataStream& stream) -> QDataStream&{ // from
                         lastSerializedFrom = 1;

                         stream >> v1;
                         stream >> responce;

                         stream >> v2;

                         return stream;
                     },
                     [this](QDataStream& stream) -> QDataStream&{ // to
                         lastSerializedto = 1;

                         stream << v1;
                         stream << responce;

                         stream << v2;
                         return stream;
                     }
                 }
             },
             }
            ) {};
};

class SingleVersionPkg: public QH::PKG::AbstractData, public MultiVersionPkg {
    QH_PACKAGE("MultiVersionPkg")

    // StreamBase interface
protected:
    QDataStream &fromStream(QDataStream &stream) override {
        lastSerializedFrom = 0;
        stream >> v1;
        stream >> responce;

        return stream;
    };
    QDataStream &toStream(QDataStream &stream) const override {
        stream << v1;
        stream << responce;

        return stream;
    };
};

class MultiVersionPkg2: public QH::PKG::MultiversionData, public MultiVersionPkg {
    QH_PACKAGE("MultiVersionPkg")

public:
    MultiVersionPkg2():  QH::PKG::MultiversionData(
            {
                {0, // version 0
                    {
                        [this](QDataStream& stream) -> QDataStream&{ // from
                            lastSerializedFrom = 0;
                            stream >> v1;
                            stream >> responce;

                            return stream;
                        },
                        [this](QDataStream& stream) -> QDataStream&{ // to
                            lastSerializedto = 0;

                            stream << v1;
                            stream << responce;

                            return stream;
                        }
                    }
                },
                {1, // version 1
                    {
                        [this](QDataStream& stream) -> QDataStream&{ // from
                            lastSerializedFrom = 1;

                            stream >> v1;
                            stream >> responce;

                            stream >> v2;

                            return stream;
                        },
                        [this](QDataStream& stream) -> QDataStream&{ // to
                            lastSerializedto = 1;

                            stream << v1;
                            stream << responce;

                            stream << v2;
                            return stream;
                        }
                    }
                },
                {2, // version 2
                    {
                        [this](QDataStream& stream) -> QDataStream&{ // from
                            lastSerializedFrom = 2;

                            stream >> v2;
                            stream >> responce;

                            return stream;
                        },
                        [this](QDataStream& stream) -> QDataStream&{ // to
                            lastSerializedto = 2;

                            stream << v2;
                            stream << responce;

                            return stream;
                        }
                    }
                }
            }
            ) {};
};

template <class Base>
class TestAPI: public QH::iParser {
public:
    TestAPI(QH::AbstractNode* parentNode): QH::iParser(parentNode) {
        registerPackageType<Base>();
    }

    // iParser interface
public:

    QH::ParserResult parsePackage(const QSharedPointer<QH::PKG::AbstractData> &pkg,
                                  const QH::Header &hdr,
                                  QH::AbstractNodeInfo *sender) override {

        if (pkg->cmd() == Base::command()) {
            data = pkg;

            if (!data.dynamicCast<MultiVersionPkg>()->responce) {
                data.dynamicCast<MultiVersionPkg>()->responce = true;
                if (!sendData(pkg.data(), sender, &hdr)) {
                    return QH::ParserResult::Error;
                }
            }

            return QH::ParserResult::Processed;
        }

        return QH::ParserResult::NotProcessed;
    };
    int version() const override {return 0;};
    QString parserId() const override {return "BigDataTestParser";};

    QSharedPointer<QH::PKG::AbstractData> getData() const{
        return data;
    };

    void dropData() {
        data = nullptr;
    };

private:
    QSharedPointer<QH::PKG::AbstractData> data = nullptr;

};

template <class Base>
class TestingClient: public QH::AbstractNode {

    // AbstractNode interface
public:
    TestingClient() {
        _parser = addApiParser<TestAPI<Base>>();
    }

    bool sendRequest(const Base& data, int port) {
        return sendData(&data, QH::HostAddress(TEST_LOCAL_HOST, port));
    }

    NodeType nodeType() const override {
        return NodeType::Node;
    };

    const QSharedPointer<QH::iParser>& parser() const {
        return _parser;
    }

private:
    QSharedPointer<QH::iParser> _parser;
};

MultiVersionTest::MultiVersionTest() {
    _nodeV1 = new TestingClient<MultiVersionPkg1>;
    _nodeV2 = new TestingClient<MultiVersionPkg2>;
    _nodeOld = new TestingClient<SingleVersionPkg>;

}

MultiVersionTest::~MultiVersionTest() {
    _nodeV1->softDelete();
    _nodeV2->softDelete();
    _nodeOld->softDelete();

}

void MultiVersionTest::test() {
    QVERIFY(_nodeV1->run(TEST_LOCAL_HOST, LOCAL_TEST_PORT));

    QVERIFY(connectFunc(_nodeV2, TEST_LOCAL_HOST, LOCAL_TEST_PORT));
    QVERIFY(connectFunc(_nodeOld, TEST_LOCAL_HOST, LOCAL_TEST_PORT));

    testMultipacakges();
    testSinglePackages();
}

void MultiVersionTest::testMultipacakges() {
    auto nodeWithV1 = dynamic_cast<TestingClient<MultiVersionPkg1>*>(_nodeV1);
    auto nodeWithV2 = dynamic_cast<TestingClient<MultiVersionPkg2>*>(_nodeV2);

    // clean old data;
    nodeWithV1->parser().staticCast<TestAPI<MultiVersionPkg1>>()->dropData();
    nodeWithV2->parser().staticCast<TestAPI<MultiVersionPkg2>>()->dropData();

    // from new to old
    {

        // initialize test values.
        MultiVersionPkg2 pkg;
        pkg.v1 = 10;
        pkg.v2 = 20;

        // send test values into dist node
        QVERIFY(nodeWithV2->sendRequest(pkg, LOCAL_TEST_PORT));

        // should use the maximum available serialization. it is 1
        QVERIFY(pkg.lastSerializedto == 1);

        // wait for receive packge on distanation node
        QVERIFY(wait([&nodeWithV1]() {
            if (auto data = nodeWithV1->parser().staticCast<TestAPI<MultiVersionPkg1>>()->
                            getData().staticCast<MultiVersionPkg1>()) {
                return data->lastSerializedFrom == 1;
            }
            return false;

        }, WAIT_RESPOCE_TIME));

        // wait for package on sender node.
        QVERIFY(wait([&nodeWithV2]() {
            if (auto data = nodeWithV2->parser().staticCast<TestAPI<MultiVersionPkg1>>()->
                            getData().staticCast<MultiVersionPkg1>()) {
                return data->responce == true;
            }
            return false;

        }, WAIT_RESPOCE_TIME));

        // chec data all data should not be changed.
        auto data = nodeWithV2->parser().staticCast<TestAPI<MultiVersionPkg2>>()->
                    getData().staticCast<MultiVersionPkg2>();

        QVERIFY(data->v1 ==  pkg.v1);
        QVERIFY(data->v2 ==  pkg.v2);
    }
}

void MultiVersionTest::testSinglePackages() {
    auto nodeWithSingle = dynamic_cast<TestingClient<SingleVersionPkg>*>(_nodeOld);
    auto nodeWithV1 = dynamic_cast<TestingClient<MultiVersionPkg1>*>(_nodeV1);

    // clean old data;
    nodeWithSingle->parser().staticCast<TestAPI<SingleVersionPkg>>()->dropData();
    nodeWithV1->parser().staticCast<TestAPI<MultiVersionPkg1>>()->dropData();

    // from single package to new

    {
        // initialize test values.
        SingleVersionPkg pkg;
        pkg.v1 = 10;
        pkg.v2 = 20;

        // send test values into dist node
        QVERIFY(nodeWithSingle->sendRequest(pkg, LOCAL_TEST_PORT));

        // should use the maximum available serialization. it is 0
        QVERIFY(pkg.lastSerializedto == 0);

        // wait for receive packge on distanation node
        QVERIFY(wait([&nodeWithV1]() {
            if (auto data = nodeWithV1->parser().staticCast<TestAPI<MultiVersionPkg1>>()->
                            getData().staticCast<MultiVersionPkg1>()) {
                return data->lastSerializedFrom == 0;
            }
            return false;

        }, WAIT_RESPOCE_TIME));

        // wait for package on sender node.
        QVERIFY(wait([&nodeWithSingle]() {
            if (auto data = nodeWithSingle->parser().staticCast<TestAPI<SingleVersionPkg>>()->
                            getData().staticCast<SingleVersionPkg>()) {
                return data->responce == true;
            }
            return false;

        }, WAIT_RESPOCE_TIME));

        // chec data all data should not be changed.
        auto data = nodeWithSingle->parser().staticCast<TestAPI<SingleVersionPkg>>()->
                    getData().staticCast<SingleVersionPkg>();

        QVERIFY(data->v1 ==  pkg.v1);
        // v2 is not supported on the v0 parser
        QVERIFY(data->v2 ==  0);
    }
}
