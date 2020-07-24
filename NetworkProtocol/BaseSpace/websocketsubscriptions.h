/*
 * Copyright (C) 2018-2020 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
*/

#ifndef WEBSOCKETSUBSCRIPTIONS_H
#define WEBSOCKETSUBSCRIPTIONS_H

#include "abstractdata.h"
#include "senderdata.h"

#include <QSet>
#include <dbaddress.h>


namespace NP {

/**
 * @brief The WebSocketSubscriptions class
 */
class NETWORKPROTOCOLSHARED_EXPORT WebSocketSubscriptions: public AbstractData, public SenderData
{
public:
    WebSocketSubscriptions();
    WebSocketSubscriptions(const Package& package);

    QDataStream &fromStream(QDataStream &stream) override;
    QDataStream &toStream(QDataStream &stream) const override;

    QSet<DbAddress> addresses() const;
    void setAddresses(const QSet<DbAddress> &addresses);

private:
    QSet<DbAddress> _addresses;


};

}
#endif // WEBSOCKETSUBSCRIPTIONS_H
