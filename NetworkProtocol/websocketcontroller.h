#ifndef WEBSOCKETCONTROLLER_H
#define WEBSOCKETCONTROLLER_H

#include <QHash>
#include <QSharedPointer>
#include <dbobject.h>


namespace NetworkProtocol {

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
    bool subscribe(QSharedPointer<AbstractNodeInfo> subscriber,
                   const DbAddress &item);

    void unsubscribe(QSharedPointer<AbstractNodeInfo> subscriber,
                     const DbAddress &item);

    const QSet<DbAddress> &list(QSharedPointer<AbstractNodeInfo> node);

public slots:
    void handleItemChanged(const QWeakPointer<DBObject>& item);

private:
    void foreachSubscribers(const QSharedPointer<DBObject> &item,
                            const QSet<QSharedPointer<AbstractNodeInfo>> &subscribersList);

    QHash<DbAddress, QSet<QSharedPointer<AbstractNodeInfo>>> _subscribs;
    QHash<QSharedPointer<AbstractNodeInfo>, QSet<DbAddress>> _items;

    AbstractNode *_node = nullptr;

};

}
#endif // WEBSOCKETCONTROLLER_H
