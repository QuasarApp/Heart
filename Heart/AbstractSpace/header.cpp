/*
 * Copyright (C) 2018-2020 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
*/

#include "header.h"

#include <QString>

namespace QH {
Header::Header() {
    reset();
}

bool Header::isValid() const {
    return command && size && hash;
}

void Header::reset() {
    size = 0;
    command = 0;
    triggerHash = 0;
    hash = 0;
}

QString Header::toString() const {
    return QString("Header description: Size - %0, Command - %1, hash - %2, triggerHash - %3").
            arg(size).arg(command).arg(QString::number(hash, 16)).arg(QString::number(triggerHash, 16));
}
}
