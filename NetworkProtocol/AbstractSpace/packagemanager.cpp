#include "packagemanager.h"

#include <package.h>
#include <ctime>
#include <config.h>

namespace NP {

PackageManager::PackageManager()
{

}

PackageManager::~PackageManager() {
    for (const auto& data : _parseResults) {
        delete data;
    }

    _parseResults.clear();
}

const Package *PackageManager::getPkgFromArhive(const BaseId &id) const {
    if (!contains(id))
        return nullptr;

    QMutexLocker lock(&_processMutex);
    return _parseResults.value(id)->_data;
}

bool PackageManager::contains(const BaseId &id) const {
    QMutexLocker lock(&_processMutex);
    return _parseResults.contains(id);
}

void PackageManager::processed(const Package &pkg, char processResult) {

    if (!pkg.id.isValid() || !PACKAGE_CACHE_SIZE) {
        return;
    }

    QMutexLocker lock(&_processMutex);

    if (_parseResults.size() > PACKAGE_CACHE_SIZE) {
        _processTime.erase(_processTime.begin());
    }

    _parseResults.insert(pkg.id, new PackaData {
                             processResult,
                             new Package(pkg)
                         });

    _processTime.insert(static_cast<int>(time(0)), pkg.id);
}

PackageManager::PackaData::~PackaData() {
    delete _data;
}

}
