/*
 * Copyright (C) 2018-2021 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
*/

#ifndef CONNECTIONINFO_H
#define CONNECTIONINFO_H

#include "abstractnodeinfo.h"
#include "accesstoken.h"
#include "heart_global.h"
#include <QByteArray>

class QAbstractSocket;
namespace QH {

class DbAddress;

/**
 * @brief The BaseNodeInfo class contaisn the unique node id.
 * This object created in the DataBaseNode::createNodeInfo method.
 */
class HEARTSHARED_EXPORT BaseNodeInfo: public AbstractNodeInfo {

public:

    /**
     * @brief BaseNodeInfo This constructor initialize the node info object from the tcp descriptor
     * @param tcp This is  tcp socket dsscriptor
     * @param clientAddress This is network address of socket.
     */
    explicit BaseNodeInfo(QAbstractSocket * tcp = nullptr,
                          const HostAddress* clientAddress = nullptr);
    ~BaseNodeInfo() override;

    bool isValid() const override;

    /**
     * @brief token This method return id of peer node or client.
     * @return The id of peer node or client.
     */
    const AccessToken& token() const;

    /**
     * @brief setToken This method set an token for the peer node or the client.
     * @param token new value of token of the peer node.
     */
    void setToken(const AccessToken &token);

    void reset() override;
    bool confirmData() const override;

    /**
     * @brief id This method return id of the connected user.
     * @return user id
     */
    const QVariant &id() const;

    /**
     * @brief setId This method set userId for connected object.
     * @param id This is new value of user id.
     */
    void setId(const QVariant &id);

protected:
    AccessToken _token;
    QVariant _id;

};

}

#endif // CONNECTIONINFO_H
