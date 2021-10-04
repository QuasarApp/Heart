/*
 * Copyright (C) 2018-2021 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
*/

#include "heart.h"

#include <QDataStream>
#include <QVariantMap>

namespace QH {

bool init() {
    initResources();
    return true;
}

QString heartLibVersion() {
    return HEART_VERSION;
}

}
