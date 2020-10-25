#ifndef PERMISIONCONTROLMEMBER_H
#define PERMISIONCONTROLMEMBER_H

#include <networkmember.h>

namespace QH {
namespace PKG {

/**
 * @brief The PermisionControlMember class Using unly for change permision for Network node object.
 */
class PermisionControlMember: public NetworkMember
{
public:
    PermisionControlMember();
    PermisionControlMember(const BaseId& id);

    // DBObject interface
public:
    bool isCached() const override;

protected:
    BaseId generateId() const override;

    // DBObject interface
public:
    DBObject *createDBObject() const override;
};

}
}
#endif // PERMISIONCONTROLMEMBER_H
