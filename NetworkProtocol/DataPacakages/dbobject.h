#ifndef DBOBJECT_H
#define DBOBJECT_H
#include <QVariantMap>
#include <dbtablebase.h>
#include "abstractdata.h"
#include "networkprotocol_global.h"

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

    /**
     * @brief getMap
     * @return map of object
     */
    QVariantMap getMap() const;
    /**
     * @brief seyMap
     * @return set map for this object
     */
    void setMap(const QVariantMap &map);

    QString tableName() const;

    void setTableName(const QString &tableName);

    /**
     * @brief factory
     * @return self object pointer
     */
    virtual QSharedPointer<DBObject> factory() = 0;

protected:

    QString _tableName;
    int _id = -1;

    //// StreamBase interface
    QDataStream &fromStream(QDataStream &stream) override;
    QDataStream &toStream(QDataStream &stream) const override;

    /**
     * @brief fromVariantMap
     * @param map
     * @return
     */
    virtual void fromVariantMap(const QVariantMap &map);

    /**
     * @brief toVariantMap
     * @param map
     * @return
     */
    virtual QVariantMap& toVariantMap(QVariantMap& map) const;


};
}


#endif // DBOBJECT_H
