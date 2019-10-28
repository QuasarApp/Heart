#ifndef IOBJECTPROVIDER_H
#define IOBJECTPROVIDER_H
#include "clientprotocol_global.h"

#include <QSharedPointer>

namespace ClientProtocol {

class DBObject;

class CLIENTPROTOCOLSHARED_EXPORT iObjectProvider
{
public:
    iObjectProvider();
    virtual ~iObjectProvider();

    /**
     * @brief getObject
     * @return
     */
    virtual bool getObject(const QString &table, int id, QSharedPointer<DBObject> result) = 0;

    /**
     * @brief saveObject
     * @return
     */
    virtual bool saveObject(QSharedPointer<DBObject> saveObject) = 0;

    /**
     * @brief deleteObject
     * @return
     */
    virtual bool deleteObject(const QString &table, int id) = 0;

};

}
#endif // IOBJECTPROVIDER_H
