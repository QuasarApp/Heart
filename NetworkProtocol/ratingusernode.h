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
    ParserResult parsePackage(const Package &pkg, const QWeakPointer<AbstractNodeInfo> &sender) override;
    QVariantMap defaultDbParams() const override;
    bool workWithUserRequest(const QWeakPointer<AbstractData> &,
                             const QHostAddress &addere,
                             const Header *rHeader);

private:

    /**
     * @brief registerNewUser
     * @param user
     * @param address
     * @return
     */
    bool registerNewUser(const QWeakPointer<AbstractData> &user,
                         const QHostAddress &address);

    /**
     * @brief loginUser
     * @param user
     * @param address
     * @return
     */
    bool loginUser(const QWeakPointer<AbstractData> &user,
                   const QWeakPointer<AbstractData> &userdb,
                   const QHostAddress &address);
};

}
#endif // RATINGUSERSERVER_H
