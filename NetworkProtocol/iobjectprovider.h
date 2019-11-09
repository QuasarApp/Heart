#ifndef IOBJECTPROVIDER_H
#define IOBJECTPROVIDER_H
#include "networkprotocol_global.h"

#include <QSharedPointer>

namespace NetworkProtocol {

class DBObject;

class NETWORKPROTOCOLSHARED_EXPORT iObjectProvider
{
public:
    iObjectProvider();
    virtual ~iObjectProvider();

    /**
     * @brief getObject
     * @return
     */
    virtual bool getObject(const QString &table, int id, QSharedPointer<DBObject> *result) = 0;

    /**
     * @brief getObject
     * @return list of pointers to findet objects
     */
    virtual bool getObjects(const QString &table, const QString& key,
                            QVariant val, QList<QSharedPointer<DBObject>> &result) = 0;

    /**
     * @brief saveObject
     * @return
     */
    virtual bool saveObject(QWeakPointer<DBObject> saveObject) = 0;

    /**
     * @brief deleteObject
     * @return
     */
    virtual bool deleteObject(const QString &table, int id) = 0;

};

}
#endif // IOBJECTPROVIDER_H
