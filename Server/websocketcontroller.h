#ifndef WEBSOCKETCONTROLLER_H
#define WEBSOCKETCONTROLLER_H
#include <QObject>
#include <clientprotocol.h>
#include <QSet>
#include <QHash>

class Item;
class PlayerDBData;

namespace ClientProtocol {
    class Server;
}

template <class KEY_TYPE, class VALUE_TYPE>
using MultiHash = QHash<KEY_TYPE, QSet<VALUE_TYPE>>;

class WebSocketController: public QObject
{
    Q_OBJECT
private:
    MultiHash<char, quint32> _subscribs;
    MultiHash<int, quint32> _subscribsObjIds;

    ClientProtocol::Server *_serverDaemon = nullptr;

    void foreachSubscribers(const Item &newData, const QSet<quint32> &subscribersList);
    void foreachSubscribers(const PlayerDBData &newData, const QSet<quint32> &subscribersList);

public:
    WebSocketController(ClientProtocol::Server * server, QObject* obj = nullptr);
    void subscribe(quint32 address, ClientProtocol::Command cmd, int id);
    void unsubscribe(quint32 address, ClientProtocol::Command cmd, int id);
public slots:
    void handleItemChanged(int id, const Item& newData);
    void handlePlayerChanged(int id, const PlayerDBData& newData);

};

#endif // WEBSOCKETCONTROLLER_H
