#ifndef CONNECTIONINFO_H
#define CONNECTIONINFO_H

#include "clientprotocol_global.h"

#include <QByteArray>

class QAbstractSocket;
namespace ClientProtocol {

#define NOT_VALID_CARMA 0xFF
#define DEFAULT_KARMA   100
#define RESTORE_KARMA   20
#define BANED_KARMA     0

class CLIENTPROTOCOLSHARED_EXPORT Connectioninfo {

    QAbstractSocket *sct = nullptr;
    int karma = DEFAULT_KARMA;
    QByteArray token;

public:
    void disconnect();

    void ban();
    bool isBaned() const;
    void unBan();

    bool isValid() const;

    Connectioninfo(QAbstractSocket * tcp = nullptr,
                   int kar = NOT_VALID_CARMA);
    ~Connectioninfo();

    int getKarma() const;
    void setKarma(int value);
    QAbstractSocket *getSct() const;
    void setSct(QAbstractSocket *value);
    QByteArray getToken() const;
    void setToken(const QByteArray &value);
};
}
#endif // CONNECTIONINFO_H
