#ifndef CONNECTIONINFO_H
#define CONNECTIONINFO_H

#include "abstractnodeinfo.h"
#include "accesstoken.h"
#include "networkprotocol_global.h"

#include <QByteArray>

class QAbstractSocket;
namespace NetworkProtocol {

/**
 * @brief The Permission enum
 * permision to data in database
 */
enum class Permission {
    NoPermission = 0x00,
    Read = 0x01,
    Write = 0x02,
};

/**
 * @brief The BaseNodeInfo class with tocken support
 */
class NETWORKPROTOCOLSHARED_EXPORT BaseNodeInfo: public AbstractNodeInfo {

public:
    bool isValid() const override;

    explicit BaseNodeInfo(QAbstractSocket * tcp = nullptr);
    ~BaseNodeInfo() override;

    /**
     * @brief token
     * @return token
     */
    const AccessToken& token() const;

    /**
     * @brief setToken
     * @param token set token
     */
    void setToken(const AccessToken &token);

    /**
     * @brief permision return permision on table item of node
     * @param table name of table
     * @param id of item,
     * @return return permision
     */
    Permission permision(const QString& table, int id) const;

    /**
     * @brief setPermision - set new permision for table object
     * @param table - table of set permision
     * @param id - id of object( set -1 if you need set permision for all items of table)
     * @param permision new value of permision
     */
    void setPermision(const QString& table, int id ,const Permission &permision);

protected:
    AccessToken _token;
    QHash<QString, QHash<int, Permission>> _permision;


};
}

#endif // CONNECTIONINFO_H
