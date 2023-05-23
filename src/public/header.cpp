/*
 * Copyright (C) 2018-2023 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
*/

#include "header.h"
#include "package.h"
#include <QString>

namespace QH {

Header::Header() {
    reset();
}

bool Header::isValid() const {
    if (size > Package::maximumSize()) {
        return false;
    }
    return command && hash && headerVersion == 1 && unusedSpace1 == 0;
}

void Header::reset() {
    size = 0;
    command = 0;
    triggerHash = 0;
    hash = 0;
    packageVersion = 0;
    unusedSpace1 = 0;
    unusedSpace2 = 0;

}

QString Header::toString() const {
    return QString("Header description: HeaderVersion - %0, PackageVersion - %1, Size - %2, Command - %3, hash - %4, triggerHash - %5").
        arg(headerVersion).arg(packageVersion).arg(size).arg(command).arg(QString::number(hash, 16), QString::number(triggerHash, 16));
}
}
