#ifndef BASENODE_H
#define BASENODE_H

#include "abstractnode.h"

#include <dbobject.h>

namespace NetworkProtocol {

class SqlDBCache;
class SqlDBWriter;
class UserData;
class UserDataRequest;
class AvailableDataRequest;
class WebSocket;
class WebSocketController;
/**
 * @brief The BaseNode class - base inplementation of nodes
 */
class BaseNode : public AbstractNode
{
    Q_OBJECT
public:

    /**
     * @brief BaseNode
     * @param mode
     * @param ptr
     */
    BaseNode(SslMode mode = SslMode::NoSSL, QObject * ptr = nullptr);

    /**
     * @brief intSqlDb - this function init database of node
     * @param DBparamsFile - path to json file with all patarams
     * @param cache - new SqlDBCache object
     * @param writer - new SqlDBWriter
     * @return true if all good
     */
    virtual bool intSqlDb( QString DBparamsFile = "",
                           SqlDBCache * cache = nullptr,
                           SqlDBWriter* writer = nullptr);

    /**
     * @brief isSqlInited
     * @return return true if intSqlDb invocked correctly;
     */
    bool isSqlInited() const;

    /**
     * @brief run server on address an port
     * @param addres
     * @param port
     * @return recomendet befor invoke this method call the intSqlDb.
     * If you skeap a call of intSqlDb method then data base inited with default parameters.
     */
    bool run(const QString &addres, unsigned short port) override;

    ~BaseNode() override;

    /**
     * @brief defaultDbParams
     * @return
     */
    virtual QVariantMap defaultDbParams() const;

signals:
    void incomingData(QSharedPointer<AbstractData> pkg,
                      const QHostAddress&  sender);

    void requestError(QString msg);

protected:

    /**
     * @brief initDefaultDbObjects create default cache and db writer if pointer is null
     * @param cache
     * @param writer
     */
    void initDefaultDbObjects(SqlDBCache *cache, SqlDBWriter *writer);

    /**
     * @brief parsePackage
     * @param pkg
     * @param sender
     * @return
     */
    ParserResult parsePackage(const Package &pkg,
                              const QWeakPointer<AbstractNodeInfo> &sender) override;


    /**
     * @brief hashgenerator
     * @param pass
     */
    virtual QString hashgenerator(const QByteArray &pass);

    QSharedPointer<AbstractNodeInfo> createNodeInfo(QAbstractSocket *socket) const override;

    QWeakPointer<SqlDBCache> db() const;

    bool workWithSubscribe(const QWeakPointer<AbstractData> &rec,
                           const QHostAddress &address);

private:
    QSharedPointer<SqlDBCache> _db;
    bool workWithAvailableDataRequest(const QWeakPointer<AbstractData> &rec,
                                      const QHostAddress &addere,
                                      const Header *rHeader);

    WebSocketController *_webSocketWorker = nullptr;
};

}
#endif // BASENODE_H
