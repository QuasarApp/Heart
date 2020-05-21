#ifndef WEBSOCKET_H
#define WEBSOCKET_H

#include "dbobject.h"
#include "request.h"


namespace NP {

/**
 * @brief The WebSocketRequest enum
 */
enum class WebSocketRequest {
    /// Invalied data
    Invalied = 0,
    /// subscribe to data
    Subscribe = 1,

    /// unsubscribe
    Unsubscribe = 2,

    /// get list of curennt subscribes
    SubscribeList = 3
};

/**
 * @brief The WebSocket class - this class contains methods for work with stream data
 */
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
