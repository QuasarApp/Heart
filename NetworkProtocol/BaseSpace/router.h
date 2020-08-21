#ifndef ROUTER_H
#define ROUTER_H
#include <QHash>
#include "baseid.h"
#include <QMutex>
#include <hostaddress.h>
#include <QMap>

namespace NP {

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
    QList<HostAddress> getRoute(const BaseId& node) const;

    /**
     * @brief contains - check contains route fo node id.
     * @param node - id of node
     * @return true if route findet.
     */
    bool contains(const BaseId& node) const;


    /**
     * @brief updateRoute - set a new route for a node if the new route is shorter than the old route
     * @param node
     * @param route
     */
    void updateRoute(const BaseId& node, const QList<HostAddress>& route);

    /**
     * @brief addProcesedPackage - add id of processed package
     * @param id
     */
    void addProcesedPackage(const BaseId& id);

    /**
     * @brief isProcessed
     * @param id
     * @return true if package is processed
     */
    bool isProcessed(const BaseId& id) const;
private:
    QHash<BaseId, QList<HostAddress>> _routes;
    QMap<qint64, BaseId> _routesTimeMap;
    mutable QMutex _routesMutex;

    QSet<BaseId> _processedPackages;
    QMap<qint64, BaseId> _processedPackagesTimeMap;
    mutable QMutex _processedPackagesMutex;

};
}
#endif // ROUTER_H
