#ifndef SERVER_CP_H
#define SERVER_CP_H
#include "clientprotocol_global.h"
#include "clientprotocol.h"

#include <QTcpServer>
#include "rsakeyspool.h"
#include "connectioninfo.h"

namespace ClientProtocol {

#define CRITICAL_ERROOR -50
#define LOGICK_ERROOR   -20
#define REQUEST_ERROR   -5


class CLIENTPROTOCOLSHARED_EXPORT Server : public QTcpServer
{
    Q_OBJECT
private:
    Package _downloadPackage;
    QHash<quint32, Connectioninfo*> _connections;

    RSAKeysPool * _pool = nullptr;
    bool parsePackage(const Package &pkg, QTcpSocket * sender);
    bool sendPackage(const Package &pkg, QTcpSocket * target);
    bool registerSocket(QTcpSocket *socket);
    bool changeKarma(quint32 addresss, int diff);
    inline bool isBaned(const QTcpSocket *) const;

    int connectionsCount() const;
    bool sendPubKey(QTcpSocket *target, const QByteArray &pubKey);


private slots:
    void avelableBytes();
    void handleDisconected();

    void handleIncommingConnection();

public:
    explicit Server(RSAKeysPool * pool, QObject * ptr = nullptr);
    ~Server() override;
    bool run(const QString& ip, unsigned short port);
    void stop(bool reset = false);

    void badRequest(quint32 address, const Header &req);

    bool sendResponse(const BaseNetworkObject* resp,  quint32 address,
                      const Header *req = nullptr);
    bool sendResponse(Package *pcg, quint32 address, const Header *req = nullptr);
    bool sendResponse(const Package &pcg, quint32 address);

    void ban(quint32 target);
    void unBan(quint32 target);

    /**
     * @brief getWorkState
     * @return string of work state
     */
    QString getWorkState() const;

    /**
     * @brief connectionState
     * @return string with count users state
     */
    QString connectionState() const;

    QStringList baned() const;

    bool getRSA(quint32, RSAKeyPair & res) const;
    QByteArray getToken(quint32 address) const;
    bool setToken(quint32 address, const QByteArray &token);

signals:
    void incomingReques(Header hdr, const QByteArray &data, const quint32 &sender);
};

}
#endif // SERVER_CP_H
