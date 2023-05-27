/*
 * Copyright (C) 2023-2023 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
*/

#include "multiversiontest.h"

class MultiVersionPkg {
public:
    int v2;
    int v1;
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
                            stream >> v1;
                            return stream;
                        },
                        [this](QDataStream& stream) -> QDataStream&{ // to
                            stream << v1;

                            return stream;
                        }
                    }
                },
                {1, // version 1
                    {
                        [this](QDataStream& stream) -> QDataStream&{ // from
                            stream >> v1;
                            stream >> v2;

                            return stream;
                        },
                        [this](QDataStream& stream) -> QDataStream&{ // to
                            stream << v1;
                            stream << v2;
                            return stream;
                        }
                    }
                }
            }
            ) {};
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
                            return stream;
                        },
                        [this](QDataStream& stream) -> QDataStream&{ // to
                            lastSerializedto = 0;

                            stream << v1;

                            return stream;
                        }
                    }
                },
                {1, // version 1
                    {
                        [this](QDataStream& stream) -> QDataStream&{ // from
                           lastSerializedFrom = 1;

                            stream >> v1;
                            stream >> v2;

                            return stream;
                        },
                        [this](QDataStream& stream) -> QDataStream&{ // to
                            lastSerializedto = 1;

                            stream << v1;
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

                            return stream;
                        },
                        [this](QDataStream& stream) -> QDataStream&{ // to
                            lastSerializedto = 2;

                            stream << v2;
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
                                  const QH::Header &,
                                  QH::AbstractNodeInfo *) override {

        if (pkg->cmd() == Base::command()) {
            data = pkg;
            return QH::ParserResult::Processed;
        }

        return QH::ParserResult::NotProcessed;
    };
    int version() const override {return 0;};
    QString parserId() const override {return "BigDataTestParser";};

    QSharedPointer<QH::PKG::AbstractData> getData() const{
        return data;
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

}

void MultiVersionTest::test() {

    TestingClient<MultiVersionPkg1> nodeWithV1;
    TestingClient<MultiVersionPkg2> nodeWithV2;

    int nodev1Port = TEST_PORT + 5;
    int nodev2Port = TEST_PORT + 6;

    QVERIFY(nodeWithV1.run(TEST_LOCAL_HOST, nodev1Port));
    QVERIFY(nodeWithV2.run(TEST_LOCAL_HOST, nodev2Port));

    // from new to old
    {
        MultiVersionPkg2 pkg;
        pkg.v1 = 10;
        pkg.v2 = 20;

        QVERIFY(nodeWithV2.sendRequest(pkg, nodev1Port));

        // should use the maximum available serialization. it is 1
        QVERIFY(pkg.lastSerializedto == 1);

        QVERIFY(wait([&nodeWithV1](){
            return nodeWithV1.parser().staticCast<TestAPI<MultiVersionPkg1>>()->
                   getData().staticCast<MultiVersionPkg1>()->lastSerializedFrom == 1;
        }, WAIT_RESPOCE_TIME));

        auto data = nodeWithV1.parser().staticCast<TestAPI<MultiVersionPkg1>>()->
                    getData().staticCast<MultiVersionPkg1>();

        QVERIFY(data->v1 ==  pkg.v1);
        QVERIFY(data->v2 ==  pkg.v2);
    }

    // from old to new

    {
        MultiVersionPkg1 pkg;
        pkg.v1 = 10;
        pkg.v2 = 20;

        QVERIFY(nodeWithV1.sendRequest(pkg, nodev1Port));

        // should use the maximum available serialization. it is 1
        QVERIFY(pkg.lastSerializedto == 1);

        QVERIFY(wait([&nodeWithV2](){
            return nodeWithV2.parser().staticCast<TestAPI<MultiVersionPkg2>>()->
                   getData().staticCast<MultiVersionPkg2>()->lastSerializedFrom == 1;
        }, WAIT_RESPOCE_TIME));

        auto data = nodeWithV2.parser().staticCast<TestAPI<MultiVersionPkg2>>()->
                    getData().staticCast<MultiVersionPkg2>();

        QVERIFY(data->v1 ==  pkg.v1);
        QVERIFY(data->v2 ==  pkg.v2);
    }
}
