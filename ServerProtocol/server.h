#ifndef SERVER_SP_H
#define SERVER_SP_H
#include "serverprotocol.h"
#include "serverprotocol_global.h"
#include <QLocalServer>

class QLocalSocket;
namespace ServerProtocol {

class SERVERPROTOCOLSHARED_EXPORT Server: public QLocalServer
{
    Q_OBJECT
private:
    QLocalSocket *_client;
    Package _downloadPackage;

    void parsePackage(const Package &pkg);

private slots:
    void avelableBytes();

protected:
    void incomingConnection(quintptr socketDescriptor) override;
public:
    explicit Server(QObject * ptr = nullptr);
    ~Server() override;
    bool run(const QString& name, bool force = false);
    bool sendResponce(QVariantMap res, Command command);

    bool sendPackage(Package &pkg);
signals:
    void incomingRequest(QVariantMap data);
};
}
#endif // SERVER_SP_H
