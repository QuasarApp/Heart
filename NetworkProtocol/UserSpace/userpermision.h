#ifndef USERPERMISION_H
#define USERPERMISION_H
#include "dbobject.h"
#include "permisions.h"

namespace NP {


/**
 * @brief The UserPermision class
 * this object contains data about permision.
 * this objcet do not stremed on network.
 */
class NETWORKPROTOCOLSHARED_EXPORT UserPermision: public DBObject
{
public:
    explicit UserPermision();

    /**
     * @brief UserPermision - this implementation call a setData method of this class.
     * @param key = permison data key
     * @param value - permision value
     */
    explicit UserPermision(const PermisionData &key, Permission value = Permission::NoPermission);

    DBObject* factory() override;

    /**
     * @brief select - this implementation get all permisons data from user or object
     * @param q
     * @return true if sunctions finished successful
     */
    bool select(QSqlQuery &q) override;

    /**
     * @brief save - this implementation write to database all permisions data on this class
     * @param q
     * @return true if sunctions finished successful
     */
    bool save(QSqlQuery &q) const override;

    /**
     * @brief getCacheId - generate id from user premisionTable and objectid for hash table of cache.
     * @param userId
     * @param tabmeObject
     * @param objectId
     * @return unique id for UserPermision (UserPermision key of cache data)
     */
    static uint getCacheId(unsigned int userId, const QString& tabmeObject, unsigned int objectId);

    bool copyFrom(const AbstractData *other) override;
    void clear() override;

    /**
     * @brief isValid
     * @return true if this object have valid permision data.
     */
    bool isValid() const override;

    /**
     * @brief remove - all permision of table  corresponding to the values of the object
     * @param q
     * @return true if function finished seccessful
     */
    bool remove(QSqlQuery &q) const override;

    /**
     * @brief getData
     * @return all permision data
     */
    const QHash<PermisionData, Permission>& getData() const;

    /**
     * @brief setData - set new data for this object (this implementation rplace all data hash table)
     * @param data - permision data
     */
    void setData(const QHash<PermisionData, Permission> &data);

    /**
     * @brief setData - this implementation set only one permision data
     * @param key - pey of permision
     * @param value - new value
     */
    void setData(const PermisionData &key, Permission value = Permission::NoPermission);

    /**
     * @brief insertData - insert new data for this object
     * @param data
     */
    void insertData(const QHash<PermisionData, Permission> &data);

    /**
     * @brief removeData - remove data
     * @param data - removed data.
     */
    void removeData(const QHash<PermisionData, Permission> &data);

    /**
     * @brief isHavePermision - check permisions
     * @param key - checkable address of permision table
     * @param value - requared permisoin
     * @return true if this object have requared permision.
     */
    bool isHavePermision(const PermisionData &key, Permission value);

protected:
    QDataStream &fromStream(QDataStream &stream) override;
    QDataStream &toStream(QDataStream &stream) const override;

private:
    QHash<PermisionData, Permission> _data;

};
}


#endif // USERPERMISION_H
