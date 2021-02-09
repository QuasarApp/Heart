/*
 * Copyright (C) 2020-2021 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
*/

#ifndef TESTSINGLESERVER_H
#define TESTSINGLESERVER_H
#include <QObject>
#include <ping.h>
#include <singleserver.h>

class TestSingleServer: public QH::SingleServer {

    Q_OBJECT

    // AbstractNode interface
public:
    const QH::PKG::Ping& getPing() const;

protected:
    void incomingData(QH::PKG::AbstractData *pkg, const QH::AbstractNodeInfo*  sender) override;

private:
    QH::PKG::Ping _ping;
};

#endif // TESTSINGLESERVER_H
