#include "user.h"
namespace QH {
namespace PKG {

User::User()
{

}

DBObject *User::createDBObject() const {
    return new User();
}
}
}
