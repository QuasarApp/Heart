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
     * obj - dbobject with own query of select.
     * After the invoke this object well contain current data.
     * @return
     */
    virtual bool getObject(QWeakPointer<DBObject> obj) = 0;

//    /**
//     * @brief getObject
//     * @return list of pointers to findet objects
//     */
//    virtual bool getObjects(const QString &table, const QString& key,
//                            QVariant val, QList<QSharedPointer<DBObject>> &result) = 0;

    /**
     * @brief saveObject
     * @return
     */
    virtual bool saveObject(QSharedPointer<DBObject> saveObject) = 0;

    /**
     * @brief deleteObject
     * @return
     */
    virtual bool deleteObject(QSharedPointer<DBObject> deleteObject) = 0;

};

}
#endif // IOBJECTPROVIDER_H
