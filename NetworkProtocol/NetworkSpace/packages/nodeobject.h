#ifndef NODEOBJECT_H
#define NODEOBJECT_H

#include <networkmember.h>
#include <senderdata.h>
namespace NP {

/**
 * @brief The NodeObject class
 */
class NETWORKPROTOCOLSHARED_EXPORT NodeObject: public NetworkMember, public SenderData
{
public:
    NodeObject();
    NodeObject(const Package& pkg);

    // StreamBase interface
protected:
    QDataStream &fromStream(QDataStream &stream) override;
    QDataStream &toStream(QDataStream &stream) const override;
};
}
#endif // NODEOBJECT_H
