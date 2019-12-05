#ifndef IOBJECTPROVIDER_H
#define IOBJECTPROVIDER_H
#include "networkprotocol_global.h"

#include <QSharedPointer>

namespace NetworkProtocol {

class DBObject;
class AbstractData;

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
    virtual bool getObject(QSharedPointer<DBObject>& obj) = 0;

    /**
     * @brief saveObject
     * @return
     */
    virtual bool saveObject(const QWeakPointer<AbstractData>& saveObject) = 0;

    /**
     * @brief deleteObject
     * @return
     */
    virtual bool deleteObject(const QWeakPointer<AbstractData>& saveObject) = 0;

};

}
#endif // IOBJECTPROVIDER_H
