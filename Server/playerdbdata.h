#ifndef PLAYERDBDATA_H
#define PLAYERDBDATA_H

#include <player.h>
#include "server_global.h"

class SERVERSHARED_EXPORT PlayerDBData: public ClientProtocol::Player
{
private:
    int lastOnline = 0;
    int onlineTime = 0;
    QByteArray pass = "";


public:
    PlayerDBData();
    int getLastOnline() const;
    void setLastOnline(int value);
    int getOnlineTime() const;
    void setOnlineTime(int value);
    QByteArray getPass() const;
    void setPass(const QByteArray &value);
    QString getHexPass() const;
    void fromHexPass(const QString &passHex);
};

#endif // PLAYERDBDATA_H
