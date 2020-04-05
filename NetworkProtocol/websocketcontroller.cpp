#include "abstractnode.h"
#include "abstractnodeinfo.h"
#include "websocketcontroller.h"
#include <quasarapp.h>

namespace NP {


WebSocketController::WebSocketController(AbstractNode *node) {
    _node = node;
    assert(_node);
}

bool WebSocketController::subscribe(SP<AbstractNodeInfo> subscriber,
                                    const DbAddress &item) {

    _subscribs[item].insert(subscriber);
    _items[subscriber].insert(item);

    return true;
}

void WebSocketController::unsubscribe(SP<AbstractNodeInfo> subscriber,
                                      const DbAddress& item) {
    _subscribs[item].remove(subscriber);
    _items[subscriber].remove(item);

}

const QSet<DbAddress> &WebSocketController::list(
        SP<AbstractNodeInfo> node) {
    return _items[node];
}

void WebSocketController::handleItemChanged(const WP<AbstractData> &item) {
    auto obj = item.toStrongRef().dynamicCast<DBObject>();
    if (obj.isNull() || !obj->isValid())
        return;

    foreachSubscribers(item, _subscribs.value(obj->dbAddress()));
}

void WebSocketController::foreachSubscribers(const WP<AbstractData> &item,
                                             const QSet<SP<AbstractNodeInfo>> &subscribersList) {

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
