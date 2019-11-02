#ifndef CONNECTIONINFO_H
#define CONNECTIONINFO_H

#include "abstractnodeinfo.h"
#include "clientprotocol_global.h"

#include <QByteArray>

class QAbstractSocket;
namespace ClientProtocol {

/**
 * @brief The Permission enum
 * permision to data in database
 */
enum class Permission {
    NoPermission = 0x00,
    Read = 0x01,
    Write = 0x02,

    Own = 0x01,
    Other = 0x10,

    Logined = (Read * Own | Write * Own) |  (Read * Other),
    Guest = (Read * Other)
};

/**
 * @brief The BaseNodeInfo class with tocken support
 */
class CLIENTPROTOCOLSHARED_EXPORT BaseNodeInfo: public AbstractNodeInfo {

public:
    bool isValid() const override;

    explicit BaseNodeInfo(QAbstractSocket * tcp = nullptr,
                          NodeType type = NodeType::Client);
    ~BaseNodeInfo() override;

    /**
     * @brief token
     * @return token
     */
    QByteArray token() const;

    /**
     * @brief setToken
     * @param token set token
     */
    void setToken(const QByteArray &token);

    /**
     * @brief isLogined
     * @return true if your node logined on destanation host (this node info)
     */
    bool isLogined() const;

    Permission permision() const;
    void setPermision(const Permission &permision);

protected:
    QByteArray _token;
    Permission _permision = Permission::Guest;

};
}

#endif // CONNECTIONINFO_H
