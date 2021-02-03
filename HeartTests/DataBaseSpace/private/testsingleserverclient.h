/*
 * Copyright (C) 2020-2021 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
*/

#ifndef TESTSINGLESERVERCLIENT_H
#define TESTSINGLESERVERCLIENT_H

#include <singleserverclient.h>
#include <ping.h>


class TestSingleServerClient: public QH::SingleServerClient
{
public:
    Q_OBJECT

    // AbstractNode interface
public:
    const QH::PKG::Ping& getPing() const;
    int getLastError() const;
    void setStatus(const QH::ClientStatus &status);;

protected:
    QH::HostAddress serverAddress() const override;
    void incomingData(QH::PKG::AbstractData *pkg, const QH::HostAddress&  sender) override;

protected slots:
    void handleError(unsigned char code, const QString &) override;

private:
    QH::PKG::Ping _ping;
    int _lastError;
};

#endif // TESTSINGLESERVERCLIENT_H
