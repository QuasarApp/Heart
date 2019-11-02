#ifndef BASENODE_H
#define BASENODE_H

#include "abstractnode.h"

namespace ClientProtocol {

class SqlDBCache;
class SqlDBWriter;
class UserDataRequest;

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
    virtual bool intSqlDb( QString DBparamsFile,
                           SqlDBCache * cache = nullptr,
                           SqlDBWriter* writer = nullptr);

    /**
      * params it is map of data base params if it option is empty then params get from defaultDbParams function
    */
    virtual bool intSqlDb( QVariantMap params = {},
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
    void incomingReques(Package pkg, const QHostAddress&  sender);
    void requestError();

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
    bool parsePackage(const Package &pkg, QWeakPointer<AbstractNodeInfo> sender) override;

    /**
     * @brief workWithUserRequest
     * @return
     */
    bool workWithUserRequest(QWeakPointer<UserDataRequest>, const QHostAddress &addere,
                             const Header *rHeader = nullptr);

    QSharedPointer<AbstractNodeInfo> createNodeInfo(QAbstractSocket *socket) const override;

private:

    QSharedPointer<SqlDBCache> _db;
};

}
#endif // BASENODE_H
