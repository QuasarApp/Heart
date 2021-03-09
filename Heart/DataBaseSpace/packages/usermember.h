#ifndef USER_H
#define USER_H

#include "abstractnetworkmember.h"
#include "accesstoken.h"

#include "itoken.h"

namespace QH {


namespace PKG {


/**
 * @brief The UserMember class is some as a AbstractNetworkMember class.
 * All registered users on the singelServer have own list database object with own permisions.
 * If you want create a custom permisions table with the custom user then ovveride this class.
 *
 * @note The UserMember class has no members of its own, so it is safe to use static_cast on User <<>> AbstractNetworkMember.
 */
class UserMember: public AbstractNetworkMember
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

    void setSignToken(const AccessToken &token);
    const AccessToken &getSignToken() const;

protected:
    QDataStream &fromStream(QDataStream &stream) override;
    QDataStream &toStream(QDataStream &stream) const override;

private:

    AccessToken _token;

};
}
}
#endif // USER_H
