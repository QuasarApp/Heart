#ifndef TESTUTILS_H
#define TESTUTILS_H

#include <networkprotocol.h>

namespace NetworkProtocol {
    class Client;
}
class TestUtils
{
public:
    TestUtils();
    static bool wait(const bool &forWait, int msec);
    static bool loginFunc(NetworkProtocol::Client &cli,
                              const QString &login,
                              const QByteArray &pass,
                              bool sendResult,
                              bool loginResult);

    static bool connectFunc(NetworkProtocol::Client &cli,
                     const QString &address,
                     unsigned short port);
//    static bool getState(ServerProtocol::Client &cli, QVariantMap &state);
//    static bool unBanFunc(ServerProtocol::Client &cli, const QHostAddress &address);
//    static bool banFunc(ServerProtocol::Client &cli, const QHostAddress &address);
//    static bool reconnectFunc(ClientProtocol::Client &cli);
//    static bool registerFunc(ClientProtocol::Client &cli, const QString &login,
    //                             const QByteArray &pass, bool sendResult, bool loginResult);

};

#endif // TESTUTILS_H
