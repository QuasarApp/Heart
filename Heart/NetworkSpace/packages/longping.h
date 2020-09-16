#ifndef LONGPING_H
#define LONGPING_H

#include "ping.h"
#include <senderdata.h>

namespace QH {
namespace PKG {

/**
 * @brief The LongPing class - test class for big network with return addresse
 */
class HEARTSHARED_EXPORT LongPing: public Ping, public SenderData
{
public:
    LongPing(const BaseId &sender);
    LongPing(const Package& from);

    // AbstractData interface
    bool isValid() const override;
    bool copyFrom(const AbstractData *) override;

    // StreamBase interface
protected:
    QDataStream &fromStream(QDataStream &stream) override;
    QDataStream &toStream(QDataStream &stream) const override;

};
}
}
#endif // LONGPING_H
