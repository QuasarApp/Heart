#ifndef DBOBJECT_H
#define DBOBJECT_H
#include <QVariantMap>
#include <dbtablebase.h>
#include "abstractdata.h"
#include "networkprotocol_global.h"
#include "dbaddress.h"

class QSqlQuery;

namespace NetworkProtocol {

/**
 * @brief The DBObject class
 */
class NETWORKPROTOCOLSHARED_EXPORT DBObject : public AbstractData
{
public:
    /**
     * @brief DBObject
     */
    DBObject(const QString& tableName);

    ~DBObject() override;

    //// AbstractData interface
    bool isValid() const override;

    /**
     * @brief getId
     * @return id of objcet
     */
    int getId() const;

    /**
     * @brief getId
     * @return id of objcet
     */
    void setId(int);

    /**
     * @brief clear
     */
    virtual void clear();

    QString tableName() const;

    void setTableName(const QString &tableName);

    /**
     * @brief factory
     * @return self object pointer
     */
    virtual QSharedPointer<DBObject> factory() = 0;

    virtual bool select(QSqlQuery& q) = 0;
    virtual bool save(QSqlQuery& q) = 0;
    virtual bool remove(QSqlQuery& q) = 0;
    virtual bool isCached() const;


    DbAddress dbAddress() const;


protected:

    QString _tableName;
    int _id = -1;

    //// StreamBase interface
    QDataStream &fromStream(QDataStream &stream) override;
    QDataStream &toStream(QDataStream &stream) const override;


};
}


#endif // DBOBJECT_H
