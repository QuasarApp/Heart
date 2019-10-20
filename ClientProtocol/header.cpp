#include "header.h"

namespace ClientProtocol {
Header::Header() {
    reset();
}

bool Header::isValid() const {

    if (sizeof (*this) != 12) {
        return false;
    }

    return true;
}

void Header::reset() {
    size = 0;
    command = 0;
    triggerCommnad = 0;
}
}
