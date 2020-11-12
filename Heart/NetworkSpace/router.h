/*
 * Copyright (C) 2018-2020 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
*/


#ifndef ROUTER_H
#define ROUTER_H
#include <QHash>
#include "nodeid.h"
#include <QMutex>
#include <hostaddress.h>
#include <QMap>

namespace QH {

/**
 * @brief The Router class - this class contains routes of nodes and optimise exists routes
 *  router - is thread save class.
 */
class Router
{
public:
    Router();

    /**
     * @brief getRoute
     * @return return route to node from this node
     */
    QList<HostAddress> getRoute(const NodeId& node) const;

    /**
     * @brief contains - check contains route fo node id.
     * @param node - id of node
     * @return true if route findet.
     */
    bool contains(const NodeId& node) const;


    /**
     * @brief updateRoute - set a new route for a node if the new route is shorter than the old route
     * @param node
     * @param route
     */
    void updateRoute(const NodeId& node, const QList<HostAddress>& route);

    /**
     * @brief addProcesedPackage - add id of processed package
     * @param id
     */
    void addProcesedPackage(const NodeId& id);

    /**
     * @brief isProcessed
     * @param id
     * @return true if package is processed
     */
    bool isProcessed(const NodeId& id) const;
private:
    QHash<NodeId, QList<HostAddress>> _routes;
    QMap<qint64, NodeId> _routesTimeMap;
    mutable QMutex _routesMutex;

    QSet<NodeId> _processedPackages;
    QMap<qint64, NodeId> _processedPackagesTimeMap;
    mutable QMutex _processedPackagesMutex;

};
}
#endif // ROUTER_H
