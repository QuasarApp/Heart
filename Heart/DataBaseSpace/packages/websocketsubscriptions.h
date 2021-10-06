/*
 * Copyright (C) 2018-2021 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
*/

#ifndef WEBSOCKETSUBSCRIPTIONS_H
#define WEBSOCKETSUBSCRIPTIONS_H

#include "abstractdata.h"

#include <QSet>
#include <dbaddress.h>


namespace QH {
namespace PKG {

/**
 * @brief The WebSocketSubscriptions class is list of subscribers object.
 *  Using for transporting a informations about subscribers.
 */
class HEARTSHARED_EXPORT WebSocketSubscriptions: public AbstractData
{
    QH_PACKAGE(WebSocketSubscriptions, "WebSocketSubscriptions")

public:
    WebSocketSubscriptions();
    WebSocketSubscriptions(const Package& package);

    QDataStream &fromStream(QDataStream &stream) override;
    QDataStream &toStream(QDataStream &stream) const override;

    /**
     * @brief addresses This is list of subscribers.
     */
    QSet<unsigned int> addresses() const;

    /**
     * @brief setAddresses This method set a new list of subscribers.
     * @param addresses This is a new list of subscribers.
     */
    void setAddresses(const QSet<unsigned int> &addresses);

private:
    QSet<unsigned int> _subscribeIds;


};

}
}
#endif // WEBSOCKETSUBSCRIPTIONS_H
