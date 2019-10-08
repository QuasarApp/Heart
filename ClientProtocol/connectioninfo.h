#ifndef CONNECTIONINFO_H
#define CONNECTIONINFO_H

#include "rsakeyspool.h"
#include "clientprotocol_global.h"

class QTcpSocket;
namespace ClientProtocol {

#define NOT_VALID_CARMA 0xFF
#define DEFAULT_KARMA   100
#define RESTORE_KARMA   20
#define BANED_KARMA     0

class CLIENTPROTOCOLSHARED_EXPORT Connectioninfo {

    QTcpSocket *sct = nullptr;
    int karma = DEFAULT_KARMA;
    RSAKeyPair RSAKey;
    QByteArray token;

public:
    void disconnect();

    void ban();
    bool isBaned() const;
    void unBan();

    bool isValid() const;

    Connectioninfo(QTcpSocket * tcp = nullptr,
                   int kar = NOT_VALID_CARMA,
                   RSAKeyPair keys = RSAKeyPair());
    ~Connectioninfo();

    int getKarma() const;
    void setKarma(int value);
    RSAKeyPair getRSAKey() const;
    void setRSAKey(const RSAKeyPair &value);
    QTcpSocket *getSct() const;
    void setSct(QTcpSocket *value);
    QByteArray getToken() const;
    void setToken(const QByteArray &value);
};
}
#endif // CONNECTIONINFO_H
