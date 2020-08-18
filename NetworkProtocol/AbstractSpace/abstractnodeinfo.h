/*
 * Copyright (C) 2018-2020 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
*/

#ifndef ABSTRACTNODEINFO_H
#define ABSTRACTNODEINFO_H
#include "hostaddress.h"
#include "networkprotocol_global.h"

#include <hostaddress.h>


class QAbstractSocket;
class QHostInfo;

namespace NP {

/**
 * @brief The TrustNode enum
 */
enum class TrustNode: unsigned char {
    /// undefined node
    Undefined = 0xFF,

    /// general trust of node
    Default = 100,

    /// this trusted of unbaned nodes
    Restore = 20,

    /// node with this trusted is forbidden
    Baned = 0
};

/**
 * @brief The AbstractNodeState enum - This is status of known nodes.
 */
enum class NodeCoonectionStatus: int {
    /// This node not sent data about its envirement
    NotConnected,
    /// The node with this status has already sent data about its environment.
    Connected,
    ///  The node confirmend. Node with it status sent a information
    ///  requarement for confirm in to this node object.
    Confirmed,
};

uint qHash(NodeCoonectionStatus status);

/**
 * @brief The AbstractNodeInfo class
 */
class NETWORKPROTOCOLSHARED_EXPORT AbstractNodeInfo
{

public:

    /**
     * @brief AbstractNodeInfo
     * @param sct socket of connection
     */
    AbstractNodeInfo(QAbstractSocket *sct = nullptr);

    /**
     * @brief ~AbstractNodeInfo
     */
    virtual ~AbstractNodeInfo();

    /**
     * @brief sct
     * @return return socket of connection
     */
    QAbstractSocket *sct() const;

    /**
     * @brief disconnect disconnect from host
     */
    virtual void disconnect();

    /**
     * @brief ban this node
     */
    virtual void ban();

    /**
     * @brief isBaned
     * @return true if node baned
     */
    virtual bool isBaned() const;

    /**
     * @brief unBan
     */
    virtual void unBan();

    /**
     * @brief trust
     * @return rtust
     */
    virtual int trust() const;

    /**
     * @brief setTrust manual set value of trust
     * @param trust - new value
     */
    virtual void setTrust(int trust);

    /**
     * @brief isValid
     * @return true if all data valid
     */
    virtual bool isValid() const;

    /**
     * @brief isConnected
     * @return true if the socket connected
     */
    virtual bool isConnected() const;

    /**
     * @brief fromStream
     * @param stream
     * @return stream
     */
    virtual QDataStream& fromStream(QDataStream& stream);

    /**
     * @brief toStream
     * @param stream
     * @return stream
     */
    virtual QDataStream& toStream(QDataStream& stream) const;

    /**
     * @brief info
     * @return Host info of this node
     */
    QHostInfo *info() const;

    /**
     * @brief setInfo - set new temp info for this node
     * @param info
     */
    void setInfo(const QHostInfo &info);

    /**
     * @brief networkAddress
     * @return network adderess of node
     */
    HostAddress networkAddress() const;

    /**
     * @brief setNetworkAddress - update network address
     * @param networkAddress - new address
     */
    void setNetworkAddress(const HostAddress &networkAddress);

    /**
     * @brief status - status of node connection
     * @return connection status
     */
    NodeCoonectionStatus status() const;

    /**
     * @brief setStatus - set new value of status node
     * @param status - new status
     */
    void setStatus(const NodeCoonectionStatus &status);

    /**
     * @brief confirmData - check all data of node and return true if node is confirmed
     * @return true if node is confirmed
     */
    virtual bool confirmData() const;

protected:
    /**
     * @brief setSct
     * @param sct
     */
    void setSct(QAbstractSocket *sct);

private:

    QHostInfo *_info = nullptr;
    HostAddress _networkAddress;

    QAbstractSocket *_sct = nullptr;
    int _trust = static_cast<int>(TrustNode::Default);
    NodeCoonectionStatus _status = NodeCoonectionStatus::NotConnected;

};

}
#endif // ABSTRACTNODEINFO_H
