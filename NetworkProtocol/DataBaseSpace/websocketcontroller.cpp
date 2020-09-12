/*
 * Copyright (C) 2018-2020 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
*/

#include "databasenode.h"
#include "abstractnodeinfo.h"
#include "websocketcontroller.h"
#include <quasarapp.h>

namespace NP {


WebSocketController::WebSocketController(DataBaseNode *node) {
    _node = node;
    assert(_node);
}

bool WebSocketController::subscribe(const BaseId& subscriber,
                                    const DbAddress &item) {

    _subscribsMutex.lock();
    _subscribs[item].insert(subscriber);
    _subscribsMutex.unlock();

    _itemsMutex.lock();
    _items[subscriber].insert(item);
    _itemsMutex.unlock();

    return true;
}

void WebSocketController::unsubscribe(const BaseId &subscriber,
                                      const DbAddress& item) {
    _subscribsMutex.lock();
    _subscribs[item].remove(subscriber);
    _subscribsMutex.unlock();

    _itemsMutex.lock();
    _items[subscriber].remove(item);
    _itemsMutex.unlock();

}

const QSet<DbAddress> &WebSocketController::list(const BaseId &subscriber) {
    QMutexLocker locker(&_itemsMutex);
    return _items[subscriber];
}

void WebSocketController::handleItemChanged(const DBObject *item) {
    auto obj = dynamic_cast<const DBObject*>(item);
    if (obj)
        return;

    QMutexLocker locker(&_subscribsMutex);

    foreachSubscribers(item, _subscribs.value(obj->dbAddress()));
}

void WebSocketController::foreachSubscribers(const DBObject *item,
                                             const QSet<BaseId> &subscribersList) {

    for (const auto &subscriber : subscribersList) {

        if (!_node->sendData(item, subscriber)) {
            QuasarAppUtils::Params::log("Send update failed for " + subscriber.toBase64(),
                                               QuasarAppUtils::Warning);
        }
    }
}
}
