#include "permisioncontrolmember.h"

namespace QH {
namespace PKG {

PermisionControlMember::PermisionControlMember() {

}

PermisionControlMember::PermisionControlMember(const BaseId &id): NetworkMember(id) {

}

bool PermisionControlMember::isCached() const {
    return false;
}

BaseId PermisionControlMember::generateId() const {
    return getId();
}

DBObject *PermisionControlMember::createDBObject() const {
    return create<PermisionControlMember>();
}
}

}
