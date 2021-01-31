/*
 * Copyright (C) 2018-2021 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
*/

#ifndef SINGLESERVER_H
#define SINGLESERVER_H

#include <databasenode.h>
#include <accesstoken.h>
#include "dberrorcodes.h"

namespace QH {

namespace PKG {
class AuthRequest;
class UserMember;
}

#define REQUEST_INTERNAL_ERROR 0
#define REQUEST_LOGIN_ERROR -1

/**
 * @brief The RegisteruserResult enum
 */
enum class RegisteruserResult {
    /// User not registered because database not inited or other error occurred.
    InternalError,
    /// User not registered because user already exists.
    UserExits,
    /// User not logined because you need register user befor login.
    UserNotExits,
    /// User not logined because have an invalid password.
    UserInvalidPasswoed,
    /// User Already Logged.
    UserAlreadyLogged,
    /// User registered successful.
    Success
};

/**
 * @brief The SingleServer class This class is classic server with support all base server functions.
 * classic server support:
 *  - ssl encryption (See the SslMode enum).
 *  - multitrading working.
 *  - user registration. (See The PKG::User class)
 *  - working with dabase. (See the DBObject class)
 *
 * Examples
 *
 * \code{cpp}
 * \endcode
 *
 */
class SingleServer : public DataBaseNode
{
    Q_OBJECT
public:

    SingleServer();

    // AbstractNode interface
protected:
    /**
     * @brief registerNewUser This method add record into database about new user. After registered  this method invoke the SingleServer::loginUser method.
     * @param user This is data of new user.
     * @param info This is info about requested client.

     * @return status of registration a new user. For more information about result statuses see the RegisteruserResult enum.
     *
     * @note This method send userData with new token to the client.
     */
    virtual RegisteruserResult registerNewUser(PKG::UserMember user,
                                               const AbstractNodeInfo* info);

    /**
     * @brief loginUser This method generate the token for a requested user.
     * @param user This is user data with name and password.
     *  For login use PKG::User::getID and PKG::User::authenticationData like userName and password hash.
     * @param info This is info about requested client.
     * @return status of login a user. For more information about result statuses see the RegisteruserResult enum.
     *
     * @note This method send userData with new token to the client.
     */
    virtual RegisteruserResult loginUser(PKG::UserMember user, const AbstractNodeInfo* info);

    /**
     * @brief generateToken This method generate a new toke.
     * @param duration this is duration of valid
     * @return a new token.
     */
    AccessToken generateToken(int duration = AccessToken::Day);

    ParserResult parsePackage(const Package &pkg, const AbstractNodeInfo *sender) override;
    QByteArray hashgenerator(const QByteArray &data) override;

private:
    bool workWithUserRequest(const QSharedPointer<PKG::UserMember> &obj, const Package &pkg, const AbstractNodeInfo *sender);

};

}
#endif // SINGLESERVER_H
