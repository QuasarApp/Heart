/*
 * Copyright (C) 2018-2020 QuasarApp.
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
 * @brief The WebSocketSubscriptions class is list of subscribrs object.
 *  Using for transporting a informations about subscribes
 */
class HEARTSHARED_EXPORT WebSocketSubscriptions: public AbstractData
{
public:
    WebSocketSubscriptions();
    WebSocketSubscriptions(const Package& package);

    QDataStream &fromStream(QDataStream &stream) override;
    QDataStream &toStream(QDataStream &stream) const override;

    /**
     * @brief addresses This is list of subscribes.
     */
    QSet<DbAddress> addresses() const;

    /**
     * @brief setAddresses This method set a new list of subscribers.
     * @param addresses This is a new list of subscribers.
     */
    void setAddresses(const QSet<DbAddress> &addresses);

private:
    QSet<DbAddress> _addresses;


};

}
}
#endif // WEBSOCKETSUBSCRIPTIONS_H
