#ifndef PERMISIONCACHE_H
#define PERMISIONCACHE_H
#include "sqldbcache.h"
#include "userpermision.h"

namespace NP {

class BaseNodeInfo;

/**
 * @brief The PermisionCache class - this implementation work with user permision table for check users permisions.
 *
 */
class NETWORKPROTOCOLSHARED_EXPORT PermisionCache: public SqlDBCache
{
    Q_OBJECT
public:
    PermisionCache();

    bool checkPermision(const BaseNodeInfo& requestNode,
                        const QWeakPointer<DBObject> &saveObject) const;

    // SqlDBCache interface
protected:
    void deleteFromCache(const QWeakPointer<AbstractData> &delObj);
    void saveToCache(const QWeakPointer<AbstractData> &obj);

private:
    QHash<PermisionData, Permission> _permisions;

};
}
#endif // PERMISIONCACHE_H
