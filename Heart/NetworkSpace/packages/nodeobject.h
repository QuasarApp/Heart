#ifndef NODEOBJECT_H
#define NODEOBJECT_H

#include <networkmember.h>
#include <senderdata.h>
namespace QH {

/**
 * @brief The NodeObject class
 */
class HEARTSHARED_EXPORT NodeObject: public NetworkMember, public SenderData
{
public:
    NodeObject();
    NodeObject(const Package& pkg);

    // StreamBase interface
protected:
    QDataStream &fromStream(QDataStream &stream) override;
    QDataStream &toStream(QDataStream &stream) const override;

    // DBObject interface
public:
    DBObject *factory() const override;
};
}
#endif // NODEOBJECT_H
