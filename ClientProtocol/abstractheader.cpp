#include "abstractheader.h"

namespace ClientProtocol {
AbstractHeader::AbstractHeader() {
    reset();
}

bool AbstractHeader::isValid() const {

    if (sizeof (*this) != 4) {
        return false;
    }

    return true;
}

void AbstractHeader::reset() {
    size = 0;
    command = 0;
    triggerCommnad = 0;
}
}
