/*
 * Copyright (C) 2018-2020 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
*/

#ifndef RATINGUSERSERVER_H
#define RATINGUSERSERVER_H

#include "basenode.h"


namespace NP {


class NETWORKPROTOCOLSHARED_EXPORT RatingUserNode: public BaseNode
{
    Q_OBJECT
public:
    RatingUserNode();

    // AbstractNode interface
protected:
    ParserResult parsePackage(const Package &pkg, const AbstractNodeInfo *sender) override;
    QVariantMap defaultDbParams() const override;
    bool workWithUserRequest(const AbstractData*,
                             const QHostAddress &addere,
                             const Header *rHeader);

private:

    /**
     * @brief registerNewUser
     * @param user
     * @param address
     * @return
     */
    bool registerNewUser(const AbstractData* user,
                         const QHostAddress &address);

    /**
     * @brief loginUser
     * @param user
     * @param address
     * @return
     */
    bool loginUser(const AbstractData* user,
                   const AbstractData* userdb,
                   const QHostAddress &address);
};

}
#endif // RATINGUSERSERVER_H
