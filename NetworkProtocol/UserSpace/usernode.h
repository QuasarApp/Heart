#ifndef USERNODE_H
#define USERNODE_H

#include "basenode.h"

namespace NP {

class NETWORKPROTOCOLSHARED_EXPORT UserNode: public BaseNode
{
    Q_OBJECT
public:
    UserNode();

    // AbstractNode interface
protected:
    ParserResult parsePackage(const Package &pkg, const WP<AbstractNodeInfo> &sender) override;
    QVariantMap defaultDbParams() const override;

    bool workWithUserRequest(const QSharedPointer<UserRequest> &,
                             const QHostAddress &addere,
                             const Header *rHeader);

private:

    /**
     * @brief registerNewUser
     * @param user
     * @param address
     * @param if this value true then tocket create a mounth
     * @return true if registaration finished sevvussful
     */
    bool registerNewUser(const WP<AbstractData> &user,
                         const QHostAddress &address,
                         bool rememberMe = false);

    /**
     * @brief loginUser
     * @param user
     * @param address
     * @return
     */
    bool loginUser(const WP<AbstractData> &user,
                   const WP<AbstractData> &userdb,
                   const QHostAddress &address);
};
}
#endif // USERNODE_H
