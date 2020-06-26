/*
 * Copyright (C) 2018-2020 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
*/

#include "header.h"

namespace NP {
Header::Header() {
    reset();
}

bool Header::isValid() const {
    return command && size && sender.isValid();
}

void Header::reset() {
    size = 0;
    command = 0;
    triggerCommnad = 0;
    sender.clear();
}
}
