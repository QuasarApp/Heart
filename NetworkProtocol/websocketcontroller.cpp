#include "abstractnode.h"
#include "abstractnodeinfo.h"
#include "websocketcontroller.h"
#include <quasarapp.h>

namespace NetworkProtocol {


WebSocketController::WebSocketController(AbstractNode *node) {
    _node = node;
    assert(_node);
}

bool WebSocketController::subscribe(QSharedPointer<AbstractNodeInfo> subscriber,
                                    const DbAddress &item) {

    _subscribs[item].insert(subscriber);
    _items[subscriber].insert(item);

    return true;
}

void WebSocketController::unsubscribe(QSharedPointer<AbstractNodeInfo> subscriber,
                                      const DbAddress& item) {
    _subscribs[item].remove(subscriber);
    _items[subscriber].remove(item);

}

const QSet<DbAddress> &WebSocketController::list(
        QSharedPointer<AbstractNodeInfo> node) {
    return _items[node];
}

void WebSocketController::handleItemChanged(const QWeakPointer<DBObject> &item) {
    auto obj = item.toStrongRef();
    if (obj.isNull() || !obj->isValid())
        return;

    foreachSubscribers(item, _subscribs.value(obj->dbAddress()));
}

void WebSocketController::foreachSubscribers(const QSharedPointer<DBObject> &item,
                                             const QSet<QSharedPointer<AbstractNodeInfo>> &subscribersList) {

    for (auto &&subscriber : subscribersList) {

        if (!subscriber.isNull() && subscriber->isValid()) {
            if (!_node->sendData(item, subscriber->id())) {
                QuasarAppUtils::Params::verboseLog("Send update failed for " + subscriber->id().toString(),
                                                   QuasarAppUtils::Warning);
            }
        } else {
            unsubscribe(subscriber, item->dbAddress());
        }
    }
}
}
