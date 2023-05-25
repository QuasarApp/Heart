/*
 * Copyright (C) 2023-2023 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
*/

#include "multiversiontest.h"

class TestingClient: public QH::AbstractNode {

    // AbstractNode interface
public:
    const QH::PKG::Ping& getPing() const {
        return _ping;
    }

    NodeType nodeType() const override {
        return NodeType::Node;
    };

protected slots:
    void receivePing(const QSharedPointer<QH::PKG::Ping>& ping) override {
        _ping = *ping;
    };

private:
    QH::PKG::Ping _ping;
};

class MultiVersionPkg: public QH::PKG::AbstractData {
    QH_PACKAGE("MultiVersionPkg")

public:
    int v1 = 0;

    // StreamBase interface
protected:
    QDataStream &fromStream(QDataStream &stream) override {
        AbstractData::fromStream(stream);

        stream >> v1;
        return stream;
    };

    QDataStream &toStream(QDataStream &stream) const override{
        stream << v1;
        return stream;
    };
};

MultiVersionTest::MultiVersionTest() {

}

void MultiVersionTest::test() {

}
