#ifndef PERMISIONCACHE_H
#define PERMISIONCACHE_H
#include "sqldbcache.h"
#include "userpermision.h"

namespace NP {

class UserNodeInfo;

/**
 * @brief The PermisionCache class - this implementation work with user permision table for check users permisions.
 *
 */
class NETWORKPROTOCOLSHARED_EXPORT PermisionCache: public SqlDBCache
{
    Q_OBJECT
public:
    PermisionCache();

    bool checkPermision(const NP::UserNodeInfo &requestNode,
                        const QWeakPointer<DBObject> &saveObject,
                        Permission requiredPermision);

    bool saveObject(const QWeakPointer<AbstractData> &saveObject) override;

    // SqlDBCache interface
protected:
    void deleteFromCache(const QWeakPointer<AbstractData> &delObj) override;
    void saveToCache(const QWeakPointer<AbstractData> &obj) override;
    bool getFromCache(QSharedPointer<DBObject> &obj) override;

private:
    QHash<PermisionData, Permission> _permisions;
    QSet<PermisionData> _needToSave;
    QMutex _savePermisionLaterMutex;


    // SqlDBCache interface
    void globalUpdateDataBasePrivate(qint64 currentTime) override;

};
}
#endif // PERMISIONCACHE_H
