#ifndef USER_H
#define USER_H

#include "networkmember.h"
#include "accesstoken.h"

namespace QH {


namespace PKG {

/**
 * @brief The UserRequestType enum is types of request.
 */
enum class UserRequestType: unsigned int {
    /// This is default type. Response of server or client.
    Responce,
    /// Request to registration a new user.
    SignIn,
    /// Request to login an exists user.
    Login
};

/**
 * @brief The User class is some as a NetworkMember class.
 * All registered users on the singelServer have own list database object with own permisions.
 * If you want create a custom permisions table with the custom user then ovveride this class.
 *
 * @note The User class has no members of its own, so it is safe to use static_cast on User <<>> NetworkMember.
 */
class User: public NetworkMember
{
public:
    User();
    User(const Package &pkg);
    User(const BaseId &id);

    bool copyFrom(const AbstractData *other) override;
    bool fromSqlRecord(const QSqlRecord &q) override;
    DBObject *createDBObject() const override;
    bool isValid() const override;

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
     * @brief type This method return type of request. for more information see the UserRequestType enum class.
     * @return type of request.
     */
    UserRequestType type() const;

    /**
     * @brief setType This method set new value for request type.
     * For more information see the UserRequestType enum class.
     * @param type This is new value of type.
     */
    void setType(const UserRequestType &type);

protected:
    BaseId generateId() const override;
    QDataStream &fromStream(QDataStream &stream) override;
    QDataStream &toStream(QDataStream &stream) const override;
    DBVariantMap variantMap() const override;

private:
    AccessToken _token;
    UserRequestType _type = UserRequestType::Responce;
};
}
}
#endif // USER_H
