#ifndef WEBSOCKET_H
#define WEBSOCKET_H

#include "dbobject.h"
#include "request.h"


namespace NP {

enum class WebSocketRequest {
    Invalied = 0,
    Subscribe = 1,
    Unsubscribe = 2,
    SubscribeList = 3
};

class NETWORKPROTOCOLSHARED_EXPORT WebSocket:
        public DBObject, public Request
{
public:
    WebSocket();
    WebSocket(const Package& package);


    QDataStream &fromStream(QDataStream &stream);
    QDataStream &toStream(QDataStream &stream) const;

    bool isValid() const;

    SP<DBObject> factory();
    bool select(QSqlQuery &);
    bool save(QSqlQuery &);
    bool remove(QSqlQuery &);
};

}
#endif // WEBSOCKET_H
