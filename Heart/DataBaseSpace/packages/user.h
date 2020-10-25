#ifndef USER_H
#define USER_H

#include "networkmember.h"

namespace QH {
namespace PKG {

/**
 * @brief The User class is some as a NetworkMember class.
 * All registered users on the singelServer have own list database object with own permisions.
 * If you want create a custom permisions table with the custom user then ovveride this class.
 *
 * @note The User class has no members of its own, so it is safe to use static_cast on User <<>> NetworkMember.
 */
class User: public NetworkMember
{
public:
    User();

    // DBObject interface
public:
    DBObject *createDBObject() const override;
};
}
}
#endif // USER_H
