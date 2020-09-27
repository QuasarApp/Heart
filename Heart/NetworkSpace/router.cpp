/*
 * Copyright (C) 2018-2020 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
*/


#include "router.h"
#include <config.h>

#include <QDateTime>
#include <quasarapp.h>
namespace QH {

Router::Router() {
}

QList<HostAddress> Router::getRoute(const BaseId &node) const {
    QMutexLocker locker(&_routesMutex);
    return _routes.value(node);
}

bool Router::contains(const BaseId &node) const {
    QMutexLocker locker(&_routesMutex);
    return _routes.contains(node);
}

void Router::updateRoute(const BaseId &node, const QList<HostAddress> &route) {
    QMutexLocker locker(&_routesMutex);

    debug_assert(node.isValid());

    if (!_routes.contains(node) || _routes.value(node).size() > route.size()) {

        _routesTimeMap.insert(QDateTime::currentSecsSinceEpoch(), node);
        _routes.insert(node, route);

        if (ROUTE_CACHE_LIMIT < _processedPackages.size()) {
            _routes.remove(*_routesTimeMap.begin());
            _routesTimeMap.erase(_routesTimeMap.begin());
        }
    }
}

void Router::addProcesedPackage(const BaseId &id) {
    QMutexLocker locker(&_processedPackagesMutex);

    _processedPackages.insert(id);
    _processedPackagesTimeMap.insert(QDateTime::currentSecsSinceEpoch(), id);

    if (TRANSPORT_PACKAGES_CACHE < _processedPackages.size()) {
        _processedPackages.remove(*_processedPackagesTimeMap.begin());
        _processedPackagesTimeMap.erase(_processedPackagesTimeMap.begin());
    }
}

bool Router::isProcessed(const BaseId &id) const {
    QMutexLocker locker(&_processedPackagesMutex);

    return _processedPackages.contains(id);

}
}
