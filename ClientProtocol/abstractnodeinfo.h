#ifndef ABSTRACTNODEINFO_H
#define ABSTRACTNODEINFO_H
#include "clientprotocol_global.h"

#include <QHostAddress>


class QAbstractSocket;
namespace ClientProtocol {

/**
 * @brief The TrustNode enum
 */
enum class TrustNode: unsigned char {
    Undefined = 0xFF,
    Default = 100,
    Restore = 20,
    Baned = 0
};

/**
 * @brief The NodeType enum
 */
enum class NodeType {
    Undefined,
    Client,
    Node,
    Server
};

/**
 * @brief The AbstractNodeInfo class
 */
class CLIENTPROTOCOLSHARED_EXPORT AbstractNodeInfo
{

public:

    /**
     * @brief AbstractNodeInfo
     * @param sct socket of connection
     */
    AbstractNodeInfo(QAbstractSocket *sct = nullptr, NodeType type = NodeType::Client);

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
     * @brief id
     * @return unique id of socket
     */
    virtual QHostAddress id() const;

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
     * @brief type
     * @return
     */
    NodeType type() const;

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

protected:
    /**
     * @brief setSct
     * @param sct
     */
    void setSct(QAbstractSocket *sct);


    /**
     * @brief setType
     * @param type type of connection node
     */
    void setType(const NodeType &type);

private:

    QHostAddress _id;
    QAbstractSocket *_sct = nullptr;
    NodeType _type = NodeType::Undefined;
    int _trust = static_cast<int>(TrustNode::Default);

};

}
#endif // ABSTRACTNODEINFO_H
