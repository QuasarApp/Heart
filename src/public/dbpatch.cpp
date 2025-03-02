/*
 * Copyright (C) 2018-2025 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
*/


#include "dbpatch.h"
namespace QH {

bool DBPatch::isValid() const {
    return versionFrom < versionTo && action;
}

}
