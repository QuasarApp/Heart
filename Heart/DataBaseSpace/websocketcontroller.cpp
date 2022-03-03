/*
 * Copyright (C) 2018-2022 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
*/

#include "databasenode.h"
#include "abstractnodeinfo.h"
#include "websocketcontroller.h"
#include <quasarapp.h>
#include <permission.h>

namespace QH {

using namespace PKG;

WebSocketController::WebSocketController(DataBaseNode *node) {
    _node = node;
    assert(_node);

}

void WebSocketController::subscribe(const QVariant &subscriber,
                                    unsigned int item) {

    _subscribsMutex.lock();
    _itemsMutex.lock();

    subscribePrivate(subscriber, item);

    _itemsMutex.unlock();
    _subscribsMutex.unlock();
}

void WebSocketController::unsubscribe(const QVariant &subscriber,
                                      unsigned int item) {
    _subscribsMutex.lock();
    _itemsMutex.lock();

    unsubscribePrivate(subscriber, item);

    _itemsMutex.unlock();
    _subscribsMutex.unlock();
}

QSet<unsigned int> WebSocketController::list(const QVariant &subscriber) {
    QMutexLocker locker(&_itemsMutex);
    return _items[subscriber];
}

void WebSocketController::handleItemChanged(const QSharedPointer<ISubscribableData> &item) {

    QMutexLocker locker(&_subscribsMutex);
    foreachSubscribers(item, _subscribs.value(item->subscribeId()));
}

void WebSocketController::foreachSubscribers(const QSharedPointer<ISubscribableData> &item,
                                             const QSet<QVariant> &subscribersList) {

    for (const auto &subscriber : subscribersList) {

        auto abstractItem = item.dynamicCast<AbstractData>();

        if (!abstractItem) {
            QuasarAppUtils::Params::log("All Subsribable objects must be child classes of the AbstractData." + subscriber.toString(),
                                               QuasarAppUtils::Error);
            unsubscribePrivate(subscriber, item->subscribeId());

        }

        auto dbObject = item.dynamicCast<DBObject>();

        bool fAllowed = true;
        if (dbObject) {
            fAllowed = _node->checkPermission(subscriber, dbObject->dbAddress(), Permission::Read) ==
                    DBOperationResult::Allowed;
        }

        if (fAllowed && !_node->sendData(abstractItem.data(), subscriber)) {
            QuasarAppUtils::Params::log("Send update failed for " + subscriber.toString(),
                                               QuasarAppUtils::Warning);

            unsubscribePrivate(subscriber, item->subscribeId());
        }

        if (!fAllowed) {
            QuasarAppUtils::Params::log(QString("Internal Error. Member:%0  not have permission to object %1").
                                        arg(subscriber.toString(), abstractItem->toString()),
                                            QuasarAppUtils::Error);
            unsubscribePrivate(subscriber, item->subscribeId());
        }
    }
}

void WebSocketController::unsubscribePrivate(const QVariant &subscriber,
                                             unsigned int item) {
    _subscribs[item].remove(subscriber);
    _items[subscriber].remove(item);
}

void WebSocketController::subscribePrivate(const QVariant &subscriber,
                                           unsigned int item) {

    _subscribs[item].insert(subscriber);
    _items[subscriber].insert(item);
}
}

uint qHash(const QVariant &variant) {
    return qHash(variant.toByteArray());
}
