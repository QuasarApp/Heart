/*
 * Copyright (C) 2018-2020 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
*/

#include "dbtablebase.h"

#include <QSet>

namespace NP {


QString DbTableBase::lastIdQuery() const {
    return QString("SELECT MAX(id) FROM " + name);
}

bool DbTableBase::isInited() const {
    return keys.size();
}

bool DbTableBase::isValid() const {
    return name.size();
}

}
