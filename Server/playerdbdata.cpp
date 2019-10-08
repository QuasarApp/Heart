#include "playerdbdata.h"

int PlayerDBData::getLastOnline() const {
    return lastOnline;
}

void PlayerDBData::setLastOnline(int value) {
    lastOnline = value;
}

int PlayerDBData::getOnlineTime() const {
    return onlineTime;
}

void PlayerDBData::setOnlineTime(int value) {
    onlineTime = value;
}

QByteArray PlayerDBData::getPass() const {
    return pass;
}

void PlayerDBData::setPass(const QByteArray &value) {
    pass = value;
}

QString PlayerDBData::getHexPass() const {
    return getPass().toHex();
}

void PlayerDBData::fromHexPass(const QString &passHex) {
    setPass(QByteArray::fromHex(passHex.toLatin1()));
}


PlayerDBData::PlayerDBData()
{

}
