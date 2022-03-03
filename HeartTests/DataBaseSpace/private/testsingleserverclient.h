/*
 * Copyright (C) 2020-2022 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
*/

#ifndef TESTSINGLESERVERCLIENT_H
#define TESTSINGLESERVERCLIENT_H

#include <singleclient.h>
#include <ping.h>
#define LOCAL_TEST_PORT TEST_PORT + 3


class TestSingleServerClient: public QH::SingleClient
{
public:
    Q_OBJECT

    // AbstractNode interface
public:
    const QH::PKG::Ping& getPing() const;
    void setStatus(const QH::ClientStatus &status);;

protected:
    QPair<QString, unsigned short> serverAddress() const override;
    void incomingData(const QH::PKG::AbstractData *pkg, const QH::AbstractNodeInfo*  sender) override;

private:
    QH::PKG::Ping _ping;

    friend class SingleServerTest;
};

#endif // TESTSINGLESERVERCLIENT_H
