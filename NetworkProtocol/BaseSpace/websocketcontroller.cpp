/*
 * Copyright (C) 2018-2020 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
*/

#include "abstractnode.h"
#include "abstractnodeinfo.h"
#include "websocketcontroller.h"
#include <quasarapp.h>

namespace NP {


WebSocketController::WebSocketController(AbstractNode *node) {
    _node = node;
    assert(_node);
}

bool WebSocketController::subscribe(AbstractNodeInfo *subscriber,
                                    const DbAddress &item) {

    _subscribs[item].insert(subscriber);
    _items[subscriber].insert(item);

    return true;
}

void WebSocketController::unsubscribe(AbstractNodeInfo *subscriber,
                                      const DbAddress& item) {
    _subscribs[item].remove(subscriber);
    _items[subscriber].remove(item);

}

const QSet<DbAddress> &WebSocketController::list(AbstractNodeInfo *node) {
    return _items[node];
}

void WebSocketController::handleItemChanged(const AbstractData *item) {
    auto obj = item.toStrongRef().dynamicCast<DBObject>();
    if (obj.isNull() || !obj->isValid())
        return;

    foreachSubscribers(item, _subscribs.value(obj->dbAddress()));
}

void WebSocketController::foreachSubscribers(const AbstractData *item,
                                             const QSet<AbstractNodeInfo *> &subscribersList) {

    auto ref = item.toStrongRef().dynamicCast<DBObject>();

    if (ref.isNull())
        return;

    for (auto &&subscriber : subscribersList) {

        if (!subscriber.isNull() && subscriber->isValid()) {
            if (!_node->sendData(item, subscriber->id())) {
                QuasarAppUtils::Params::log("Send update failed for " + subscriber->id().toString(),
                                                   QuasarAppUtils::Warning);
            }
        } else {
            unsubscribe(subscriber, ref->dbAddress());
        }
    }
}
}
