#include "item.h"
#include "playerdbdata.h"
#include "websocketcontroller.h"
#include <cpserver.h>

WebSocketController::WebSocketController(ClientProtocol::Server *server, QObject* obj):
    QObject (obj) {

    assert( server );
    _serverDaemon = server;
}

void WebSocketController::subscribe(quint32 address, ClientProtocol::Command cmd, int id) {

    _subscribs[static_cast<char>(cmd)].insert(address);

    if (cmd == ClientProtocol::Command::Player || cmd == ClientProtocol::Command::GetItem) {
        _subscribsObjIds[id].insert(address);
    }
}

void WebSocketController::unsubscribe(quint32 address, ClientProtocol::Command cmd, int id) {
    _subscribs[static_cast<char>(cmd)].remove(address);

    if (cmd == ClientProtocol::Command::Player || cmd == ClientProtocol::Command::GetItem) {
        _subscribsObjIds[id].remove(address);
    }
}

void WebSocketController::foreachSubscribers(const Item &newData,
                                             const QSet<quint32> &subscribersList) {
    for (auto &&subscriber : subscribersList) {
        if (_subscribs.value(static_cast<char>(ClientProtocol::Command::GetItem)).contains(subscriber)) {
            _serverDaemon->sendResponse(newData, subscriber);
        }
    }
}

void WebSocketController::foreachSubscribers(const PlayerDBData &newData,
                                             const QSet<quint32> &subscribersList){
    for (auto &&subscriber : subscribersList) {
        if (_subscribs.value(static_cast<char>(ClientProtocol::Command::GetItem)).contains(subscriber)) {
            _serverDaemon->sendResponse(&newData, subscriber);
        }
    }
}

void WebSocketController::handleItemChanged(int id, const Item &newData) {

    foreachSubscribers(newData, _subscribsObjIds.value(id));
    foreachSubscribers(newData, _subscribsObjIds.value(-1));
}

void WebSocketController::handlePlayerChanged(int id, const PlayerDBData &newData) {
    foreachSubscribers(newData, _subscribsObjIds.value(id));
    foreachSubscribers(newData, _subscribsObjIds.value(-1));
}
