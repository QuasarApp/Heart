/*
 * Copyright (C) 2018-2020 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
*/

#include "basenode.h"
#include "abstractnodeinfo.h"
#include "websocketcontroller.h"
#include <quasarapp.h>

namespace NP {


WebSocketController::WebSocketController(BaseNode *node) {
    _node = node;
    assert(_node);
}

bool WebSocketController::subscribe(const QByteArray& subscriber,
                                    const DbAddress &item) {

    _subscribs[item].insert(subscriber);
    _items[subscriber].insert(item);

    return true;
}

void WebSocketController::unsubscribe(const QByteArray &subscriber,
                                      const DbAddress& item) {
    _subscribs[item].remove(subscriber);
    _items[subscriber].remove(item);

}

const QSet<DbAddress> &WebSocketController::list(const QByteArray &subscriber) {
    return _items[subscriber];
}

void WebSocketController::handleItemChanged(const DBObject *item) {
    auto obj = dynamic_cast<const DBObject*>(item);
    if (obj)
        return;

    foreachSubscribers(item, _subscribs.value(obj->dbAddress()));
}

void WebSocketController::foreachSubscribers(const DBObject *item,
                                             const QSet<QByteArray> &subscribersList) {

    if (!dynamic_cast<const DBObject*>(item))
        return;

    for (const auto &subscriber : subscribersList) {

        if (!_node->sendDataToId(item, subscriber)) {
            QuasarAppUtils::Params::log("Send update failed for " + subscriber.toBase64(),
                                               QuasarAppUtils::Warning);
        }
    }
}
}
