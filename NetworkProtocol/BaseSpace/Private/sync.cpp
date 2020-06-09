#include "sync.h"

#include <basenode.h>
namespace NP {

Sync::Sync(BaseNode *node) {
    _node = node;
}

NetworkStatus Sync::status() const {
    return _status;
}

void Sync::setStatus(const NetworkStatus &status) {
    _status = status;
}
}
