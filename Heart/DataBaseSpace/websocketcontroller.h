
/*
 * Copyright (C) 2018-2021 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
*/

#ifndef WEBSOCKETCONTROLLER_H
#define WEBSOCKETCONTROLLER_H

#include <QHash>
#include <QSharedPointer>
#include <dbobject.h>
#include <QMutex>

namespace QH {

class AbstractNodeInfo;
class DataBaseNode;

/**
 * @brief The WebSocketController class is manage subscribe. This class contains information about users and him subscriptions.
 */
class HEARTSHARED_EXPORT WebSocketController
{

public:
    /**
     * @brief WebSocketController default constructor.
     * @param node This is pointer to node object.
     */
    WebSocketController(DataBaseNode *node);

    /**
     * @brief subscribe This method subscribe a subscriber to the item.
     * @param subscriber This is network member that want get information about update of the item.
     * @param item This is a subscribable object id.
     * @return true if method finished successful.
     */
    void subscribe(const QVariant &subscriber,
                   unsigned int item);

    /**
     * @brief unsubscribe This method unsubscribe a subscriber from the item.
     * @param subscriber This is network member that want disable getting information about update of the item.
     * @param item This is a subscribable object id.
     * @return true if method finished successful.
     */
    void unsubscribe(const QVariant &subscriber,
                     unsigned int item);

    /**
     * @brief list This method return a list of subscribed items of subscriber.
     * @param subscriber This is network member that want get a list of own subscription.
     * @return true if method finished successful.
     */
    QSet<unsigned int> list(const QVariant& subscriber);

    /**
     * @brief handleItemChanged This method invoked when item on database changed.
     * @param item This is changed item.
     */
    void handleItemChanged(const QSharedPointer<PKG::ISubscribableData> &item);

private:
    void foreachSubscribers(const QSharedPointer<PKG::ISubscribableData> &item,
                            const QSet<QVariant> &subscribersList);

    void unsubscribePrivate(const QVariant &subscriber,
                            unsigned int item);

    void subscribePrivate(const QVariant &subscriber,
                          unsigned int item);

    /// subscribers it is nodes or clients
    QHash<unsigned int, QSet<QVariant>> _subscribs;
    QHash<QVariant, QSet<unsigned int>> _items;

    QMutex _subscribsMutex;
    QMutex _itemsMutex;

    DataBaseNode *_node = nullptr;

};

}

uint qHash(const QVariant& variant);
#endif // WEBSOCKETCONTROLLER_H
