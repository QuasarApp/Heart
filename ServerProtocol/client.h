#ifndef CLIENT_SP_H
#define CLIENT_SP_H
#include "serverprotocol.h"
#include "serverprotocol_global.h"
#include <QObject>
#include <QHostAddress>

class QLocalSocket;

namespace ServerProtocol {

class SERVERPROTOCOLSHARED_EXPORT Client : public QObject
{
    Q_OBJECT
private:
    QLocalSocket *_destination;
    Package _downloadPackage;
    bool received = false;

    bool sendPackage(const Package& pkg);

private slots:
    void incommingData();

public:
    explicit Client(const QString &server = DEFAULT_SERVER, QObject * ptr = nullptr);
    bool ping();
    bool getState();
    bool ban(const QHostAddress& address);
    bool unBan(const QHostAddress& address);
    bool restart(const QString &address, unsigned short port);
    bool start(const QString &address, unsigned short port);
    bool stop();
    bool wait(bool &forWait, int msec = 10000);
    bool wait(int msec = 10000);


signals:
    void sigIncommingData(const QVariantMap& map);
};

}


#endif // CLIENT_SP_H
