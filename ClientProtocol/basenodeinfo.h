#ifndef CONNECTIONINFO_H
#define CONNECTIONINFO_H

#include "clientprotocol_global.h"

#include <QByteArray>

class QAbstractSocket;
namespace ClientProtocol {

#define NOT_VALID_CARMA 0xFF
#define DEFAULT_KARMA   100
#define RESTORE_KARMA   20
#define BANED_KARMA     0

enum class Relations {
    Undefined,
    Client,
    Node,
    Server
};

class CLIENTPROTOCOLSHARED_EXPORT BaseNodeInfo {

private:
    QAbstractSocket *_sct = nullptr;
    Relations _rel = Relations::Undefined;
protected:

    int karma = DEFAULT_KARMA;
    QByteArray token;

public:
    /**
     * @brief disconnect disconnect from host
     */
    void disconnect();

    /**
     * @brief id
     * @return unique id of socket
     */
    unsigned int id() const;

    /**
     * @brief ban this node
     */
    void ban();

    /**
     * @brief isBaned
     * @return true if node baned
     */
    bool isBaned() const;

    /**
     * @brief unBan
     */
    void unBan();

    /**
     * @brief isValid
     * @return true if node is valid
     */
    virtual bool isValid() const;

    /**
     * @brief BaseNodeInfo
     * @param tcp socket
     * @param kar karma of node
     */
    explicit BaseNodeInfo(QAbstractSocket * tcp = nullptr,
                   int kar = NOT_VALID_CARMA);
    virtual ~BaseNodeInfo();

    /**
     * @brief getKarma
     * @return node karma
     */
    int getKarma() const;

    /**
     * @brief setKarma
     * @param value new node karma
     */
    void setKarma(int value);

    /**
     * @brief getSct
     * @return socket
     */
    QAbstractSocket *getSct() const;

    /**
     * @brief setSct
     * @param value - new socket
     */
    void setSct(QAbstractSocket *value);

    /**
     * @brief getToken
     * @return tocken of node ()
     */
    QByteArray getToken() const;

    /**
     * @brief setToken
     * @param valuenew tocken
     */
    void setToken(const QByteArray &value);

    /**
     * @brief getRel
     * @return Relation of node
     */
    Relations getRel() const;

    /**
     * @brief setRel
     * @param rel
     */
    void setRel(const Relations &rel);
};
}

size_t qHash(const ClientProtocol::BaseNodeInfo& info);
#endif // CONNECTIONINFO_H
