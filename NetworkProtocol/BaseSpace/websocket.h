/*
 * Copyright (C) 2018-2020 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
*/

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


    QDataStream &fromStream(QDataStream &stream) override;
    QDataStream &toStream(QDataStream &stream) const override;

    bool isValid() const override;

    DBObject* factory() override;
    bool select(QSqlQuery &) override;
    bool save(QSqlQuery &) const override;
    bool remove(QSqlQuery &) const override;
};

}
#endif // WEBSOCKET_H
