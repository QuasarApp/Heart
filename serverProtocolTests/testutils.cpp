#include "testutils.h"

#include <QCoreApplication>
#include <QDateTime>
#include <QVariantMap>

auto terminalFuncPrivate(std::function<bool()> func,
                         ServerProtocol::Client& cli, QVariantMap* resMap = nullptr) {
    bool isWork = false;
    bool received = false;

    QMetaObject::Connection m_connection;

    m_connection = QObject::connect(&cli, &ServerProtocol::Client::sigIncommingData,
                     [&isWork, &received, &m_connection, resMap] (const QVariantMap& map) {

        isWork = !map.contains("Error");
        received = true;

        if (resMap)
            *resMap = map;

        QObject::disconnect(m_connection);
    });


    if (!func()) {
        return false;
    }

    if (!cli.wait(received, 1000)) {
        return false;
    }

    return isWork;
}

bool clientFuncPrivate(std::function<bool()> requestFunc,
            ClientProtocol::Client& cli,
            ClientProtocol::Command* responceCmd = nullptr,
            QByteArray* responceArray = nullptr) {

        bool received = false;
        QMetaObject::Connection m_connection;
        m_connection = QObject::connect(&cli, &ClientProtocol::Client::sigIncommingData,
                         [ &received, &m_connection, responceCmd, responceArray]
                                        (const ClientProtocol::Command cmd,
                                        const QByteArray& data) {

            received = true;

            if (responceCmd) {
                *responceCmd = cmd;
            }

            if (responceArray) {
                *responceArray = data;
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
    bool result = clientFuncPrivate(wraper, cli);

    if (!result) {
        return !sendResult;
    }

    return loginResult == cli.isLogin();
}

bool TestUtils::registerFunc(
        ClientProtocol::Client &cli,
                    const QString& login,
                    const QByteArray& pass,
                    bool sendResult,
                    bool loginResult) {

    auto wraper = [&cli, login, pass](){return cli.registration(login, pass);};
    bool result = clientFuncPrivate(wraper, cli);

    if (!result) {
        return !sendResult;
    }

    return loginResult == cli.isLogin();
}


bool TestUtils::getState( ServerProtocol::Client& cli, QVariantMap &state) {
    auto wraper = [&cli](){return cli.getState();};
    return terminalFuncPrivate(wraper, cli, &state);
}

bool TestUtils::unBanFunc( ServerProtocol::Client& cli, const QHostAddress& address) {
    auto wraper = [&cli, address](){return cli.unBan(address);};
    return terminalFuncPrivate(wraper, cli);
}


bool TestUtils::banFunc( ServerProtocol::Client& cli, const QHostAddress& address) {
    auto wraper = [&cli, address](){return cli.ban(address);};
    return terminalFuncPrivate(wraper, cli);
}

bool TestUtils::reconnectFunc(ClientProtocol::Client &cli) {
    cli.reconnectToHost();
    return wait(cli.isOnline(), 1000);
}
