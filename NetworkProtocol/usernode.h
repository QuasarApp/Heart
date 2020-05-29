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

    bool workWithUserRequest(const WP<AbstractData> &,
                             const QHostAddress &addere,
                             const Header *rHeader);

private:

    /**
     * @brief registerNewUser
     * @param user
     * @param address
     * @return
     */
    bool registerNewUser(const WP<AbstractData> &user,
                         const QHostAddress &address);

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
