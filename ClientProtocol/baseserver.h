#ifndef SERVER_CP_H
#define SERVER_CP_H
#include "clientprotocol_global.h"
#include "clientprotocol.h"

#include <QTcpServer>
#include "basenodeinfo.h"

namespace ClientProtocol {

#define CRITICAL_ERROOR -50
#define LOGICK_ERROOR   -20
#define REQUEST_ERROR   -5


class CLIENTPROTOCOLSHARED_EXPORT BaseServer : public QTcpServer
{
    Q_OBJECT
private:
    BasePackage _downloadPackage;
    QHash<quint32, AbstractNode*> _connections;

    bool parsePackage(const BasePackage &pkg, QAbstractSocket * sender);
    bool sendPackage(const BasePackage &pkg, QAbstractSocket *target);
    bool registerSocket(QAbstractSocket *socket);
    bool changeKarma(quint32 addresss, int diff);
    inline bool isBaned(const QTcpSocket *) const;

    int connectionsCount() const;


private slots:
    void avelableBytes();
    void handleDisconected();

    void handleIncommingConnection();

public:
    explicit BaseServer(QObject * ptr = nullptr);
    ~BaseServer() override;
    bool run(const QString& ip, unsigned short port);
    void stop(bool reset = false);

    void badRequest(quint32 address, const BaseHeader &req);

    bool sendResponse(const BaseNetworkObject* resp,  quint32 address,
                      const BaseHeader *req = nullptr);
    bool sendResponse(BasePackage *pcg, quint32 address, const BaseHeader *req = nullptr);
    bool sendResponse(const BasePackage &pcg, quint32 address);

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

    QByteArray getToken(quint32 address) const;
    bool setToken(quint32 address, const QByteArray &token);

signals:
    void incomingReques(BaseHeader hdr, const QByteArray &data, const quint32 &sender);
};

}
#endif // SERVER_CP_H
