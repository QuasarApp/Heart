
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
    bool subscribe(AbstractNodeInfo* subscriber,
                   const DbAddress &item);

    void unsubscribe(AbstractNodeInfo* subscriber,
                     const DbAddress &item);

    const QSet<DbAddress> &list(AbstractNodeInfo* node);

public slots:
    void handleItemChanged(const AbstractData* item);

private:
    void foreachSubscribers(const AbstractData *item,
                            const QSet<AbstractNodeInfo*> &subscribersList);

    QHash<DbAddress, QSet<AbstractNodeInfo*>> _subscribs;
    QHash<AbstractNodeInfo*, QSet<DbAddress>> _items;

    AbstractNode *_node = nullptr;

};

}
#endif // WEBSOCKETCONTROLLER_H
