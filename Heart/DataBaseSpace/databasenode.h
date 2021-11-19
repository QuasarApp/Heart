/*
 * Copyright (C) 2018-2021 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
*/

#ifndef DATABASENODE_H
#define DATABASENODE_H

#include "abstractnode.h"
#include <dbobject.h>
#include <hostaddress.h>
#include <permission.h>


namespace QH {

namespace PKG {
class WebSocket;
class ISubscribableData;

}

class ISqlDBCache;
class SqlDBWriter;
class WebSocketController;
class DbAddress;
class NodeId;
class iObjectProvider;


/**
 * @brief DBPatch This is function that should be upgrade database.
 * @see DBPatchMap
 * @see DataBaseNode::dbPatch
 */
typedef std::function<bool (const QH::iObjectProvider *)> DBPatch;
/**
 * @brief DBPatchMap This is list when index of list is version of database and value if function that should be upgrade database.
 * @see DataBaseNode::dbPatch
 * @see DBPatchMap
 */
typedef QList<DBPatch> DBPatchMap;

/**
 * @brief The BaseNode class is database base implementation of nodes or servers.
 *  This implementation contains methods for work with database.
 *  DataBaseNode is thread save class.
 * @see DBObject
 */
class HEARTSHARED_EXPORT DataBaseNode : public AbstractNode
{
    Q_OBJECT
public:

    DataBaseNode(QObject * ptr = nullptr);
    ~DataBaseNode() override;

    /**
     * @brief intSqlDb This method initalize database of this node or server.
     * @param DBparamsFile This is path to json file with all params of database.
     * For more information of available parameters see the SqlDBWriter::defaultInitPararm method.
     * @param cache This is pointer to the custom child of SqlDBCache class.
     * IF you set nullptr value of this parameter then  well be created a default SqlDBCache object.
     * @param writer This is pointer tot the custom child of SqlDBWriter class.
     * If you set nullptr value of this parameter then well be created a default AsyncSqlDbWriter object.
     * @return True if the database initialized successful.
     */
    virtual bool initSqlDb( QString DBparamsFile = "",
                            ISqlDBCache * cache = nullptr,
                            SqlDBWriter* writer = nullptr);

    /**
     * @brief isSqlInited This method return true if database initialized successful.
     * @return True if database initialized successful.
     */
    bool isSqlInited() const;

    bool run(const QString &addres, unsigned short port) override;

    /**
     * @brief run This method is some as AbstractNode::run but set for node custom work folder.
     * This maybe use for multiple deployment nodes on one host.
     * @param addres This is network address of work node or server.
     *  If address is empty then server will be listen all addresses of all interfaces else listen only selected address.
     * @param port This is port of deployment node (server).
     * @return Result of deployment node (sever). (True if deploy finished successful else false).
     * @param localNodeName This is name of node and  work folder of node.
     */
    virtual bool run(const QString &addres, unsigned short port,
                     const QString &localNodeName);

    void stop() override;

    /**
     * @brief defaultDbParams This method return default database parameters for this node.
     * Override this method for set new default parameters. Or use json database parameters file in DataBaseNode::run.
     * @return Map with all database parameters.
     *  For more information of available parameters see the SqlDBWriter::defaultInitPararm method.
     */
    virtual QVariantMap defaultDbParams() const;

    /**
     * @brief sendData This method is some as AbstractNode::sendData but it try send data to the id.
     *  This implementation do not prepare object to sending.
     * @param resp This is sending object to the nodeId.
     * @param nodeId This is id of target node.
     * @param req This is header of request.
     * @return true if a data send successful.
     */
    virtual unsigned int sendData(const PKG::AbstractData *resp, const QVariant &nodeId,
                          const Header *req = nullptr);

    unsigned int sendData(const PKG::AbstractData *resp, const HostAddress &nodeId,
                  const Header *req = nullptr) override;

    unsigned int sendData(const PKG::AbstractData *resp, const AbstractNodeInfo *node,
                  const Header *req = nullptr) override;

    /**
     * @brief deleteObject This method delete object by database address.
     *  @note If you want to delete any object use only this method because this method check permission of requester to execute this action.
     * @param requester This is pointer to network member that send this request.
     * @param dbObject This is pointer to object of database to remove.
     * @return result of operation (allow, forbidden, unknown).
     *  For more information about results see the DBOperationResult enum.
     */
    DBOperationResult deleteObject(const QVariant &requester,
                                   const QSharedPointer<PKG::DBObject> &dbObject);

    /**
     * @brief getObject This method try get an object by database address.
     *  @note If you want to get any object use only this method because this method check permission of requester to execute this action
     * @param requester This is pointer to network member that send this request.
     * @param templateObj This is pointer to object of database with data for generation the sql select request.
     * @param result This is a shared pointer for save result of request.
     * @return result of operation (allow, forbidden, unknown).
     *  For more information about results see the DBOperationResult enum.
     */
    DBOperationResult getObject(const QVariant &requester,
                                const PKG::DBObject &templateObj,
                                QSharedPointer<PKG::DBObject> &result) const;

    /**
     * @brief getObjects This method try get objects by template object.
     *  @note If you want to get any objects use only this method because this method check permission of requester to execute this action
     * @param requester This is pointer to network member that send this request.
     * @param templateObj This is pointer to object of database with data for generation the sql select request.
     * @param result This is reference to the list of result objects.
     * @return result of operation (allow, forbidden, unknown).
     *  For more information about results see the DBOperationResult enum.
     */
    DBOperationResult getObjects(const QVariant &requester,
                                 const PKG::DBObject &templateObj,
                                 QList<QSharedPointer<PKG::DBObject>> &result) const;

    /**
     * @brief updateObject This method try save or update database object.
     *  @note If you want to save or update any objects use only this method because this method check permission of requester to execute this action
     * @param requester This is network mebmer that send this request.
     * @param saveObject This is pointer to object of database for save or update.
     * @return result of operation (allow, forbidden, unknown).
     *  For more information about results see the DBOperationResult enum.
     */
    DBOperationResult updateObject(const QVariant &requester,
                                   const QSharedPointer<PKG::DBObject> &saveObject);

    /**
     * @brief createObject This method create a new object in the database and add all permissions for the objects creator.
     *  @note If you want to create any objects use only this method because this method check permission of requester to execute this action
     * @param requester This is network member that send this request.
     * @param obj This is pointer to object of database for save or update.
     * @return result of operation (allow, forbidden, unknown).
     *  For more information about results see the DBOperationResult enum.
     */
    DBOperationResult createObject(const QVariant &requester,
                                   const QSharedPointer<PKG::DBObject> &obj);

    /**
     * @brief updateIfNotExistsCreateObject This is wraper of the updateObject and createObjects methods.
     * 1. Try update object
     * 2. If object not exists Try create new object.
     * 3. Return operation result
     * @param requester This is network member that send this request.
     * @param obj This is initializing object.
     * @return result of operation (allow, forbidden, unknown).
     *  For more information about results see the DBOperationResult enum.
     */
    DBOperationResult updateIfNotExistsCreateObject(const QVariant &requester,
                                                    const QSharedPointer<PKG::DBObject> &obj);

    /**
     * @brief changeObjects This is wrapper of the "ISqlDBCache::changeObjects" method.
     *  Key difference between a base method is checking of the permision for needed action.
     * @note If you want to change any objects use only this method because this method check permission of requester to execute this action
     * @param requester This is network member that send this request.
     * @param templateObj This is pointer to object of database with data for generation the sql select request.
     * @param changeAction This is action function for change all selected objects.
     * @return result of operation (allow, forbidden, unknown).
     *  For more information about results see the DBOperationResult enum.
     */
    DBOperationResult changeObjects(const QVariant &requester,
                                    const PKG::DBObject &templateObj,
                                    const std::function<bool (const QSharedPointer<QH::PKG::DBObject>&)> &changeAction);


protected:

    /**
     * @brief setLocalNodeName This method sets new local name of database file.
     * @note This method must be invoked before the DataBaseNode::initsqlDb and the DataBaseNode::run methods.
     * @param newLocalNodeName This is new name of node object and node databae file.
     *
     */
    void setLocalNodeName(const QString &newLocalNodeName);

    /**
     * @brief initDefaultDbObjects This method create a default cache and database writer objects if the input pointers is null
     *  Override this method for create a custom database objects for your node class.
     * @note If you override this object then you no longer need to overload the run method to set your own controls.
     * This method invoked automatically when you call the DataBaseNode::run method.
     * @param cache This is Cache database object.
     * @param writer This is Database writerObject.
     */
    virtual void initDefaultDbObjects(ISqlDBCache *cache, SqlDBWriter *writer);

    ParserResult parsePackage(const QSharedPointer<PKG::AbstractData> &pkg,
                              const Header& pkgHeader,
                              const AbstractNodeInfo* sender) override;

    AbstractNodeInfo *createNodeInfo(QAbstractSocket *socket, const HostAddress *clientAddress) const override;

    bool changeTrust(const HostAddress &id, int diff) override;

    /**
     * @brief changeTrust This implementation of change trust is change trust node or user by self id.
     * All changes of trust saving into local database.
     * @param id This is id of user of other network member object.
     * @param diff This is difference of trust.
     * @return true if trust of user changed successful.
     */
    virtual bool changeTrust(const QVariant &id, int diff);

    /**
     * @brief hashgenerator This method generate a hash from any value.
     * Override this method for set your custom salt.
     * @param data This input byte array.
     */
    virtual QByteArray hashgenerator(const QByteArray &data) const;

    void nodeConnected(AbstractNodeInfo *node) override;
    void nodeDisconnected(AbstractNodeInfo *node) override;


    /**
     * @brief memberSubsribed This method invoked when client with @a clientId subscribed on object with  @a subscribeId.
     * @param clientId This is id of the client member.
     * @param subscribeId This is id of the subscribeObject.
     */
    virtual void memberSubsribed(const QVariant &clientId, unsigned int subscribeId);

    /**
     * @brief memberUnSubsribed This method invoked when client with @a clientId unsubsribed on object with  @a subscribeId
     * @param clientId This is id of the client member.
     * @param subscribeId This is id of the subscribeObject.
     */
    virtual void memberUnsubsribed(const QVariant &clientId, unsigned int subscribeId);

    /**
     * @brief db This node return pointer to database object.
     * @return The pointer to data base.
     */
    ISqlDBCache* db() const;

    /**
     * @brief getSender This method return id of requester.
     *  By Default base implementation get id from BaseNdoeInfo.
     * override this method for correctly work of the DataBaseNode::ParsePacakge method.
     * @param connectInfo This is info about connection.
     * @param requestData This is data of request.
     * @return id of requester member.
     */
    virtual QVariant getSender(const AbstractNodeInfo *connectInfo, const PKG::AbstractData *requestData) const;

    /**
     * @brief checkPermision This method check a permision of requester, to database object with objectAddress.
     *  Override this method if you have a custom database structure.
     * @param requester This is  user, node or another object of network
     * @param objectAddress This is address to database object
     * @param requarimentPermision This is needed permission for requester
     * @return DBOperationResult::Alowed if permission granted.
     *  For more information about result see the DBOperationResult enum.
     */
    virtual DBOperationResult checkPermission(const QVariant &requester,
                                              const DbAddress& objectAddress,
                                              const Permission& requarimentPermision) const;


    /**
     * @brief addUpdatePermission This method added or update permission for member.
     * @warning This method do not have a validation. It is just change a NetworkMembers table, so use this carefully.
     * @param member This is member id (user of node).
     * @param objectAddress This is database  object for which the permissions will be set.
     * @param permision This is permission level.
     * @param defaultPermision This is default permision for all of rest objects. By default This is Permission::Read
     * @return true if method finished successful.
     */
    virtual bool addUpdatePermission(const QVariant &member,
                                     const DbAddress& objectAddress,
                                     const Permission& permision,
                                     const Permission& defaultPermision = Permission::Read) const;

    /**
     * @brief removePermission This method use to removed permission for member.
     * @warning This method do not have a validation. It is just change a NetworkMembers table, so use this carefully.
     * @param member This is member id (user of node)
     * @param objectAddress This is database object for which the permissions will be removed
     * @return true if method finished successful.
     */
    virtual bool removePermission(const QVariant &member,
                                  const DbAddress& objectAddress) const;

    /**
     * @brief dbLocation This method return location of nodes or clients database.
     * @return path to the location of database.
     */
    QString dbLocation() const;

    /**
     * @brief welcomeAddress This method send to the node information about self.
     * Override this method if you want send custom data to incoming connection.
     * @param node This is info object of the peer node.
     * @return true if all information sender successful.
     */
    virtual bool welcomeAddress(AbstractNodeInfo *node);

    /**
     * @brief isBanned This method check trust of node, if node trust is lover of 0 return true.
     * @param member This is member of network (node, client or server).
     * @return true if node is banned.
     */
    bool isBanned(const QVariant &member) const;

    /**
     * @brief SQLSources This method contains list of sqldatabase sources.
     * This method will be invoked into initialize sql method and deploy sql database.
     *
     * All sql files will be deployed in QList order.
     *
     *  By Default This method deploy next sql code:
     * \code{sql}
     *  CREATE TABLE IF NOT EXISTS NetworkMembers (
            id VARCHAR(64) PRIMARY KEY NOT NULL,
            authenticationData BLOB default NULL,
            trust INTEGER default 0
        );

        CREATE TABLE IF NOT EXISTS MemberPermisions (
            memberId VARCHAR(64) NOT NULL,
            objectTable VARCHAR(100) NOT NULL,
            objectId VARCHAR(64) NOT NULL,
            lvl INTEGER NOT NULL,

            FOREIGN KEY(memberId) REFERENCES NetworkMembers(id)
                    ON UPDATE CASCADE
                    ON DELETE CASCADE

        );
        CREATE UNIQUE INDEX IF NOT EXISTS MemberPermisionsIndex ON MemberPermisions(memberId, objectTable, objectId);

     * \endcode
     * For add own sql code just override this method, but do not forget invoke a base method of a parent class.
     *
     * Example:
     * \code{cpp}
     *    return DataBaseNode::SQLSources() << "path/to/mySqlFile.sql";
     * \endcode
     *
     * @return the list to deploy sql files.
     */
    virtual QStringList SQLSources() const;

    /**
     * @brief systemTables This method return the set of tables that forbidden for users.
     * By default is NetworkMembers and MemberPermisions tables.
     * @return set of tables names.
     */
    virtual QSet<QString> systemTables() const;

    /**
     * @brief notifyObjectChanged This method send all subscriptions message with this object.
     * @param item changed object.
     * @return true if an item object sendible.
     */
    bool notifyObjectChanged(const QSharedPointer<PKG::ISubscribableData> &item);

    /**
     * @brief objectRemoved This method invoked when object with @a address removed from database.
     *  Oberride this method for handle this event. Default implementation do nothing.
     * @param address This is address of the deteted object.
     */
    virtual void objectRemoved(const DbAddress& address);

    /**
     * @brief objectChanged This method invoked when object with @a address changed in database.
     *  Override this method for handle this event. Default implementation do nothing.
     * @param obj This is address of the changed object.
     */
    virtual void objectChanged(const QSharedPointer<PKG::DBObject>& obj);

    /**
     * @brief dbPatches This method should be return map with functions that upgrade production data base.
     *  Eeach function shoul be can upgrade databae from preview version to own version.
     *  **Example**:
     *
     *  We have 4 version of data base {0, 1, 2, 3} each version should be contains own function for upgrade data base.
     *  Where the 0 version is first version of database. (genesis)
     *
     *  @code{cpp}
     *    QH::DBPatchMap dbPatches() const {
              QH::DBPatchMap result;

              result += [](const QH::iObjectProvider* database) -> bool {
                  // Some code for update from 0 to 1
              };

              result += [](const QH::iObjectProvider* database) -> bool {
                  // Some code for update from 1 to 2
              };

              result += [](const QH::iObjectProvider* database) -> bool {
                  // Some code for update from 2 to 3
              };

              return result;
          }
     *  @endcode
     *
     * @return Map of database pactches.
     *
     * @see DBPatchMap
     * @see DBPatch
     */
    virtual DBPatchMap dbPatches() const;

private slots:
    void handleObjectChanged(const QSharedPointer<PKG::DBObject> &item);
    void handleObjectDeleted(const DbAddress &item);


private:
    /**
         * @brief workWithSubscribe This method work with subscribe commnads.
         * @param rec This is request data.
         * @param address This is sender address.
         * @return true if data parsed successful.
         */
    bool workWithSubscribe(const PKG::WebSocket &rec,
                           const QVariant &clientOrNodeid,
                           const AbstractNodeInfo *sender);


    bool isForbidenTable(const QString& table);

    /**
     * @brief upgradeDataBase This method upgrade data base to actyaly database version.
     * @note The last version of dbPatches is actyaly version.
     * @return true if operation finished successful
     */
    bool upgradeDataBase();

    ISqlDBCache *_db = nullptr;
    QString _localNodeName;
    WebSocketController *_webSocketWorker = nullptr;


    friend class WebSocketController;

};


}

Q_DECLARE_METATYPE(QSharedPointer<QH::PKG::DBObject>)
#endif // DATABASENODE_H
