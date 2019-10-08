#ifndef WEBSOCKET_H
#define WEBSOCKET_H
#include "updateplayerdata.h"
#include "clientprotocol.h"

namespace ClientProtocol {

#pragma pack(push, 1)
struct WebSocketData {
    unsigned char cmd : 5;
    unsigned subscribe : 1;
    int objectId : 26; // -1 is all ovjects, or other bumber is number of object

    friend QDataStream& operator<< (QDataStream& stream, WebSocketData data);
    friend QDataStream& operator>> (QDataStream& stream, WebSocketData& data);
};
#pragma pack(pop)

class CLIENTPROTOCOLSHARED_EXPORT WebSocket : public UpdatePlayerData
{
    WebSocketData _data;
public:
    WebSocket();

    BaseNetworkObject *create() const override;
    NetworkClassSize classSize() const override;
    QDataStream &writeToStream(QDataStream &stream) const override;
    QDataStream &readFromStream(QDataStream &stream) override;
    bool isValid() const override;

    bool isSubscribe() const;
    Command getCommand() const;
    int getObjectId() const;

    void setSubscribe(bool subscribe);
    void setCommand(Command cmd) ;
    void setObjectId(int value);

};
}
#endif // WEBSOCKET_H
