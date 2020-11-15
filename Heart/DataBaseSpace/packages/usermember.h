#ifndef USER_H
#define USER_H

#include "networkmember.h"
#include "accesstoken.h"

namespace QH {


namespace PKG {


/**
 * @brief The UserMember class is some as a NetworkMember class.
 * All registered users on the singelServer have own list database object with own permisions.
 * If you want create a custom permisions table with the custom user then ovveride this class.
 *
 * @note The UserMember class has no members of its own, so it is safe to use static_cast on User <<>> NetworkMember.
 */
class UserMember: public NetworkMember
{
public:
    UserMember();
    UserMember(const Package &pkg);
    UserMember(const QVariant &id);

    bool copyFrom(const AbstractData *other) override;
    bool fromSqlRecord(const QSqlRecord &q) override;
    DBObject *createDBObject() const override;
    bool isValid() const override;
    DBVariantMap variantMap() const override;

    // DBObject interface
    /**
     * @brief token This method return access token of this user
     * @return access token of this user.
     */
    AccessToken token() const;

    /**
     * @brief setToken This method set new value for access token of this user
     * @param token This is new value for access token of this user
     */
    void setToken(const AccessToken &token);

    /**
     * @brief name This is username.
     * @return user name
     */
    QString name() const;

    /**
     * @brief setName This method sets new name for user.
     * @param name This is a new value of naeme.
     */
    void setName(const QString &name);

protected:
    QDataStream &fromStream(QDataStream &stream) override;
    QDataStream &toStream(QDataStream &stream) const override;

private:
    AccessToken _token;
    QString _name;

};
}
}
#endif // USER_H
