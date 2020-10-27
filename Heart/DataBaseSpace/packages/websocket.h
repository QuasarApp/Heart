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


namespace QH {
namespace PKG {

/**
 * @brief The WebSocketRequest enum This enum is list of available request of WebSocket functions.
 */
enum class WebSocketRequest {
    /// This is value for reserve invalied data. All request must be not equal 0.
    Invalied = 0,
    /// Subscribe to changes of the databases object.
    Subscribe = 1,

    /// Subscribe to changes of the databases object.
    Unsubscribe = 2,

    /// send request of get a list of curennt subscribes.
    SubscribeList = 3
};

/**
 * @brief The WebSocket class is contains methods for work with stream data.
 */
class HEARTSHARED_EXPORT WebSocket:
        public AbstractData, public Request
{
public:
    WebSocket();
    WebSocket(const Package& package);

    bool isValid() const override;

    /**
     * @brief address This method return address of the subscribe object.
     * @return database address of the subscribe object
     */
    DbAddress address() const;

    /**
     * @brief setAddress This method set a address of the subscribe object.
     * @param address This is a new database address of the subscribe object.
     */
    void setAddress(const DbAddress &address);

protected:
    QDataStream &fromStream(QDataStream &stream) override;
    QDataStream &toStream(QDataStream &stream) const override;

private:
    DbAddress _address;
    WebSocketRequest _request = WebSocketRequest::Invalied;
    // Request interface
public:
    unsigned char getRequestCmd() const override;
};

}
}
#endif // WEBSOCKET_H
