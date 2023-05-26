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

class MultiVersionPkg: public QH::PKG::MultiversionData {
    QH_PACKAGE("MultiVersionPkg")

public:
    MultiVersionPkg():  QH::PKG::MultiversionData(
            {
                {0, // version 0
                    {
                        [this](QDataStream& stream){ // from
                            stream >> v1;
                            return stream;
                        },
                        [this](QDataStream& stream){ // to
                            stream << v1;

                            return stream;
                        }
                    }
                },
                {1, // version 1
                    {
                        [this](const QDataStream* stream){ // from
                            stream >> v1;
                            stream >> v2;

                            return stream;
                        },
                        [this](const QDataStream* stream){ // to
                            stream << v1;
                            stream << v2;
                            return stream;
                        }
                    }
                }
            }
            ) {};
    int v1;
    int v2;

};

MultiVersionTest::MultiVersionTest() {

}

void MultiVersionTest::test() {

}
