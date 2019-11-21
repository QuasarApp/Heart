#ifndef RATINGUSERSERVER_H
#define RATINGUSERSERVER_H

#include "basenode.h"


namespace NetworkProtocol {


class RatingUserNode: public BaseNode
{
    Q_OBJECT
public:
    RatingUserNode();

    // AbstractNode interface
protected:
    bool parsePackage(const Package &pkg, QWeakPointer<AbstractNodeInfo> sender) override;
    QVariantMap defaultDbParams() const override;
    bool workWithUserRequest(QWeakPointer<UserDataRequest>, const QHostAddress &addere, const Header *rHeader);

private:

    /**
     * @brief registerNewUser
     * @param user
     * @param address
     * @return
     */
    bool registerNewUser(QWeakPointer<UserDataRequest> user,
                         const QHostAddress &address);

    /**
     * @brief loginUser
     * @param user
     * @param address
     * @return
     */
    bool loginUser(const QWeakPointer<UserDataRequest> &user,
                   const QWeakPointer<DBObject> &userdb, const QHostAddress &address);
};

}
#endif // RATINGUSERSERVER_H
