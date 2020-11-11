
/*
 * Copyright (C) 2018-2020 QuasarApp.
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
 * @brief The WebSocketController class is manage subscribe. This class contains information about users and him subscriptions
 */
class HEARTSHARED_EXPORT WebSocketController : public QObject
{
    Q_OBJECT

public:
    /**
     * @brief WebSocketController default construector.
     * @param node This is pointer to node object.
     */
    WebSocketController(DataBaseNode *node);

    /**
     * @brief subscribe This method subscribe a subscriber to the item.
     * @param subscriber This is network member that want get information about update of the item.
     * @param item This is database object.
     * @return true if method finished succesful
     */
    bool subscribe(const QVariant &subscriber,
                   const DbAddress &item);

    /**
     * @brief unsubscribe This method unsubscribe a subscriber from the item.
     * @param subscriber This is network member that want disable getting information about update of the item.
     * @param item This is database object.
     * @return true if method finished succesful
     */
    void unsubscribe(const QVariant &subscriber,
                     const DbAddress &item);

    /**
     * @brief list This method return a list of subscribed items of subscriber.
     * @param subscriber This is network member that want get alist of own subscription.
     * @return true if method finished succesful
     */
    const QSet<DbAddress> &list(const QVariant& subscriber);

public slots:
    /**
     * @brief handleItemChanged This method invoked when item on database changed.
     * @param item This is changed item.
     */
    void handleItemChanged(const PKG::DBObject *item);

private:
    void foreachSubscribers(const PKG::DBObject *item,
                            const QSet<QVariant> &subscribersList);

    /// subscribers it is nodes or clients
    QHash<DbAddress, QSet<QVariant>> _subscribs;
    QHash<QVariant, QSet<DbAddress>> _items;

    QMutex _subscribsMutex;
    QMutex _itemsMutex;

    DataBaseNode *_node = nullptr;

};

}
#endif // WEBSOCKETCONTROLLER_H
