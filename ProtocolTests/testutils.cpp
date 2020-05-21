#include "testutils.h"

#include <QCoreApplication>
#include <QDateTime>
#include <QVariantMap>
#include <basenode.h>
#include <client.h>

bool funcPrivate(std::function<bool()> requestFunc,
                 NP::BaseNode* node,
                 SP<NP::AbstractData>* responce = nullptr,
                 QHostAddress *responceSender = nullptr) {

    bool received = false;
    QMetaObject::Connection m_connection;
    m_connection = QObject::connect(node, &NP::BaseNode::incomingData,
                                    [ &received, responce, responceSender]
                                    (SP<NP::AbstractData> pkg,
                                    const QHostAddress& sender) {

        received = true;

        if (responce) {
            *responce = pkg;
        }

        if (responceSender) {
            *responceSender = sender;
        }

    });

    if (!requestFunc()) {
        return false;
    }

    if (!TestUtils::wait(received, 10000))
        return false;

    QObject::disconnect(m_connection);


    return true;
}


bool funcPrivateConnect(std::function<bool()> requestFunc,
                        NP::Client* node) {

    bool connected = false;
    QMetaObject::Connection m_connection;
    m_connection = QObject::connect(node, &NP::Client::statusChanged,
                                    [ &connected](int new_status) {

        connected = NP::Client::Status::Online == static_cast<NP::Client::Status>(new_status);

    });

    if (!requestFunc()) {
        return false;
    }

    TestUtils::wait(connected, 10900);
    QObject::disconnect(m_connection);

    return connected;
}

TestUtils::TestUtils()
{

}

bool TestUtils::wait(const bool &forWait, int msec) {
    auto curmsec = QDateTime::currentMSecsSinceEpoch() + msec;
    while (curmsec > QDateTime::currentMSecsSinceEpoch() && !forWait) {
        QCoreApplication::processEvents();
    }
    QCoreApplication::processEvents();
    return forWait;
}

bool TestUtils::loginFunc(
        NP::Client *cli,
        const QString& login,
        const QByteArray& pass,
        bool sendResult,
        bool loginResult) {

    auto wraper = [cli, login, pass](){return cli->login(login, pass);};
    bool result = funcPrivate(wraper, cli);

    if (!result) {
        return !sendResult;
    }

    return loginResult == (cli->status() == NP::Client::Logined);
}

bool TestUtils::connectFunc(
        NP::Client *cli,
        const QString& address,
        unsigned short port) {

    auto wraper = [&cli, address, port](){
        cli->setHost(QHostAddress(address), port);
        cli->connectClient();
        return true;
    };

    return funcPrivateConnect(wraper, cli);
}

//bool TestUtils::getState( ServerProtocol::Client& cli, QVariantMap &state) {
//    auto wraper = [&cli](){return cli.getState();};
//    return terminalFuncPrivate(wraper, cli, &state);
//}

//bool TestUtils::unBanFunc( ServerProtocol::Client& cli, const QHostAddress& address) {
//    auto wraper = [&cli, address](){return cli.unBan(address);};
//    return terminalFuncPrivate(wraper, cli);
//}


//bool TestUtils::banFunc( ServerProtocol::Client& cli, const QHostAddress& address) {
//    auto wraper = [&cli, address](){return cli.ban(address);};
//    return terminalFuncPrivate(wraper, cli);
//}

//bool TestUtils::reconnectFunc(ClientProtocol::Client &cli) {
//    cli.reconnectToHost();
//    return wait(cli.isOnline(), 1000);
//}
