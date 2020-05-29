#ifndef USERPERMISION_H
#define USERPERMISION_H
#include "dbobject.h"
#include "permisions.h"

namespace NP {

struct PermisionData {
    unsigned int _userId = 0;
    QString _objectTable;
    unsigned int _idObject = 0;

    Permission _lvl = Permission::NoPermission;

    friend bool operator == (const PermisionData& left, const PermisionData& right);
};

/**
 * @brief The UserPermision class
 * this object contains data about permision.
 * this objcet du not stremed on network.
 */
class NETWORKPROTOCOLSHARED_EXPORT UserPermision: public DBObject
{
public:
    UserPermision();

    SP<DBObject> factory() override;
    bool select(QSqlQuery &q) override;
    bool save(QSqlQuery &q) override;

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
    bool isValid() const override;

    /**
     * @brief remove - all permision of table  corresponding to the values of the object
     * @param q
     * @return true if function finished seccessful
     */
    bool remove(QSqlQuery &q) override;

    const QHash<PermisionData, Permission>& getData() const;
    void setData(const QHash<PermisionData, Permission> &data);
    void insertData(const QHash<PermisionData, Permission> &data);
    void removeData(const QHash<PermisionData, Permission> &data);

protected:
    QDataStream &fromStream(QDataStream &stream) override;
    QDataStream &toStream(QDataStream &stream) const override;

private:
    QHash<PermisionData, Permission> _data;

};
}

uint qHash(const NP::PermisionData& userPermision);;
#endif // USERPERMISION_H
