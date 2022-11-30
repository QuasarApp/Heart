/*
 * Copyright (C) 2018-2022 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
*/


#include "packagemanager.h"

#include <package.h>
#include <ctime>
#include <config.h>
#include <packadata.h>

namespace QH {

PackageManager::PackageManager()
{

}

PackageManager::~PackageManager() {
    for (const auto& data : qAsConst(_parseResults)) {
        delete data;
    }

    _parseResults.clear();
}

const Package *PackageManager::getPkgFromArhive(const unsigned int &id) const {
    if (!contains(id))
        return nullptr;

    QMutexLocker lock(&_processMutex);
    return _parseResults.value(id)->_data;
}

bool PackageManager::contains(const unsigned int &id) const {
    QMutexLocker lock(&_processMutex);
    return _parseResults.contains(id);
}

void PackageManager::processed(const Package &pkg, char processResult) {

    if (!pkg.hdr.hash || !PACKAGE_CACHE_SIZE) {
        return;
    }

    QMutexLocker lock(&_processMutex);

    if (_parseResults.size() > PACKAGE_CACHE_SIZE) {
        _processTime.erase(_processTime.cbegin());
    }

    _parseResults.insert(pkg.hdr.hash, new PackaData {
                             processResult,
                             new Package(pkg)
                         });

    _processTime.insert(static_cast<int>(time(nullptr)), pkg.hdr.hash);
}

}
