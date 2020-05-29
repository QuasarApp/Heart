
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


namespace NP {

class AbstractNodeInfo;
class AbstractNode;

/**
 * @brief The WebSocketController class - manage subscribe
 */
class NETWORKPROTOCOLSHARED_EXPORT WebSocketController : public QObject
{
    Q_OBJECT

public:
    WebSocketController(AbstractNode *node);
    bool subscribe(SP<AbstractNodeInfo> subscriber,
                   const DbAddress &item);

    void unsubscribe(SP<AbstractNodeInfo> subscriber,
                     const DbAddress &item);

    const QSet<DbAddress> &list(SP<AbstractNodeInfo> node);

public slots:
    void handleItemChanged(const WP<AbstractData> &item);

private:
    void foreachSubscribers(const WP<AbstractData> &item,
                            const QSet<SP<AbstractNodeInfo>> &subscribersList);

    QHash<DbAddress, QSet<SP<AbstractNodeInfo>>> _subscribs;
    QHash<SP<AbstractNodeInfo>, QSet<DbAddress>> _items;

    AbstractNode *_node = nullptr;

};

}
#endif // WEBSOCKETCONTROLLER_H
