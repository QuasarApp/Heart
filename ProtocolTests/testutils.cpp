#include "testutils.h"

#include <QCoreApplication>
#include <QDateTime>
#include <QVariantMap>
#include <basenode.h>
#include <client.h>

bool funcPrivate(std::function<bool()> requestFunc,
            ClientProtocol::BaseNode& node,
            QSharedPointer<ClientProtocol::AbstractData>* responce = nullptr,
            QHostAddress *responceSender = nullptr) {

        bool received = false;
        QMetaObject::Connection m_connection;
        m_connection = QObject::connect(&node, &ClientProtocol::BaseNode::incomingData,
                         [ &received, &m_connection, responce, responceSender]
                                        (QSharedPointer<ClientProtocol::AbstractData> pkg,
                                        QHostAddress sender) {

            received = true;

            if (responce) {
                *responce = pkg;
            }

            if (responceSender) {
                *responceSender = sender;
            }

            QObject::disconnect(m_connection);


        });

        if (!requestFunc()) {
            return false;
        }

        return TestUtils::wait(received, 1000);
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
        ClientProtocol::Client &cli,
                    const QString& login,
                    const QByteArray& pass,
                    bool sendResult,
                    bool loginResult) {

    auto wraper = [&cli, login, pass](){return cli.login(login, pass);};
    bool result = funcPrivate(wraper, cli);

    if (!result) {
        return !sendResult;
    }

    return loginResult == (cli.status() == ClientProtocol::Client::Logined);
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
