#ifndef SINGLESERVER_H
#define SINGLESERVER_H

#include <databasenode.h>

namespace QH {

namespace PKG {
class User;
}

/**
 * @brief The RegisteruserResult enum
 */
enum RegisteruserResult {
    /// User not registered because database not inited or other error occurred.
    InternalError,
    /// User not registered because user already exists.
    UserExits,
    /// User registered successful.
    Success
};

/**
 * @brief The SingleServer class This class is classic server with support all base server functions.
 * classic server support:
 *  - ssl encryption
 *  - multitrading working.
 *  - user registration.
 *  - working with dabase.
 *
 * Examples
 *
 * \code{cpp}
 * \code
 */
class SingleServer : public DataBaseNode
{
    Q_OBJECT
public:
    SingleServer();

    // AbstractNode interface
protected:
    ParserResult parsePackage(const Package &pkg, const AbstractNodeInfo *sender) override;

    RegisteruserResult registerNewUser(const PKG::User *user);
    RegisteruserResult loginUser(const PKG::User *user);

};

}
#endif // SINGLESERVER_H
