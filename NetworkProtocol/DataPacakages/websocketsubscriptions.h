#ifndef WEBSOCKETSUBSCRIPTIONS_H
#define WEBSOCKETSUBSCRIPTIONS_H

#include "abstractdata.h"

#include <QSet>
#include <dbaddress.h>


namespace NP {

/**
 * @brief The WebSocketSubscriptions class
 */
class NETWORKPROTOCOLSHARED_EXPORT WebSocketSubscriptions: public AbstractData
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
