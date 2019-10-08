#ifndef SERVER_H
#define SERVER_H
#include <serverprotocol.h>
#include "server_global.h"
#include <QHostAddress>
#include <clientprotocol.h>

namespace ServerProtocol {
    class Server;
}

namespace ClientProtocol {
    class Server;
    class BaseNetworkObject;
    class Login;
    class RSAKeyPair;
}
class KeysReactor;
class SqlDBCache;
class PlayerDBData;
class WebSocketController;

class SERVERSHARED_EXPORT MainServer: public QObject
{
    Q_OBJECT
private:
    ServerProtocol::Server *_terminalPort = nullptr;
    ClientProtocol::Server *_serverDaemon= nullptr;
    SqlDBCache *_db = nullptr;
    KeysReactor *_keyReactor = nullptr;
    WebSocketController* _websocketctrl = nullptr;

    bool payItem(int player, int idItem);
    bool sellItem(int player, int idItem);
    QByteArray generateTocket(const QString& gmail) const;
    QByteArray registerPlayer(const ClientProtocol::Login &login,
                        const ClientProtocol::RSAKeyPair &rsa) const;
    QByteArray loginPlayer(const ClientProtocol::Login& login,
                     const ClientProtocol::RSAKeyPair& rsa) const;

    bool restartSrver(const QString& ip, unsigned short port);

private slots:
    void handleRequest(ClientProtocol::Header hdr, const QByteArray &data,
                       const quint32& addres);
    void handleTerminalRequest(QVariantMap obj);

public:
    MainServer(bool forceKeys, QObject *ptr = nullptr);
    bool run(const QString& ip = "", unsigned short port = 0, const QString &db = "",
             const QString &terminalServer = "");
    virtual ~MainServer();

};

#endif // SERVER_H
