#ifndef UPDATEPLAYERDATA_H
#define UPDATEPLAYERDATA_H

#include "basenetworkobject.h"
namespace ClientProtocol {


class CLIENTPROTOCOLSHARED_EXPORT UpdatePlayerData : public BaseNetworkObject
{
private:
    QByteArray token;

public:
    UpdatePlayerData();
    QByteArray getToken() const;
    void setToken(const QByteArray &value);

    BaseNetworkObject *create() const override;
    NetworkClassSize classSize() const override;
    QDataStream &writeToStream(QDataStream &stream) const override;
    QDataStream &readFromStream(QDataStream &stream) override;
    bool isValid() const override;
};

}
#endif // UPDATEPLAYERDATA_H
