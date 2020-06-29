#include "testutils.h"

#include <QCoreApplication>
#include <QDateTime>
#include <QVariantMap>
#include <abstractnode.h>

bool TestUtils::funcPrivateConnect(const std::function<bool()> &requestFunc,
                                   const std::function<bool()> &checkFunc,
                                   const std::function<QMetaObject::Connection()> &connectFunction) {

    QMetaObject::Connection m_connection = connectFunction();
    if (!requestFunc()) {
        QObject::disconnect(m_connection);
        return false;
    }

    bool return_value = TestUtils::wait(checkFunc, 1000);
    QObject::disconnect(m_connection);

    return return_value;
}

bool TestUtils::funcPrivateConnect(const std::function<bool ()> &requestFunc,
                                   const std::function<bool ()> &checkFunc) {
    return funcPrivateConnect(requestFunc, checkFunc, [](){return QMetaObject::Connection();});
}

TestUtils::TestUtils()
{

}

bool TestUtils::wait(const std::function<bool()> &forWait, int msec) {
    auto curmsec = QDateTime::currentMSecsSinceEpoch() + msec;
    while (curmsec > QDateTime::currentMSecsSinceEpoch() && !forWait()) {
        QCoreApplication::processEvents();
    }
    QCoreApplication::processEvents();
    return forWait();
}

bool TestUtils::connectFunc(
        NP::AbstractNode *cli,
        const QString& address,
        unsigned short port) {

    auto wraper = [&cli, address, port]() {
        cli->connectToHost(QHostAddress(address), port);
        return true;
    };

    auto check = [&cli]() {
        return cli->connectionsCount();
    };

    return funcPrivateConnect(wraper, check, [](){ return QMetaObject::Connection{};});
}
