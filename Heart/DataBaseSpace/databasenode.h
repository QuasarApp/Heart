/*
 * Copyright (C) 2018-2020 QuasarApp.
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

}

class SqlDBCache;
class SqlDBWriter;
class WebSocketController;
class DbAddress;
class NodeId;


/**
 * @brief The BaseNode class is dabase base inplementation of nodes or servers.
 *  This implementation contains methods for work with database.
 *  DataBaseNode is thread save class
 */
class HEARTSHARED_EXPORT DataBaseNode : public AbstractNode
{
    Q_OBJECT
public:

    DataBaseNode(QObject * ptr = nullptr);
    ~DataBaseNode() override;

    /**
     * @brief intSqlDb this method init database of this node or server.
     * @param DBparamsFile This is path to json file with all patarams of database.
     * For more information of available parameters see the SqlDBWriter::defaultInitPararm method.
     * @param cache This is pointer to the custom child of SqlDBCache class.
     * IF you set nullptr value ofr this parameter then  well be created a default SqlDBCache object.
     * @param writer This is pointer tot the custom child of SqlDBWriter class.
     * If you set nullptr value of this parameter then well be created a default AsyncSqlDbWriter object.
     * @return true if the database initalized successful.
     */
    virtual bool initSqlDb( QString DBparamsFile = "",
                            SqlDBCache * cache = nullptr,
                            SqlDBWriter* writer = nullptr);

    /**
     * @brief isSqlInited This method return true if database initialized successful.
     * @return true if database initialized successful.s
     */
    bool isSqlInited() const;

    bool run(const QString &addres, unsigned short port) override;

    /**
     * @brief run This metod is some as AbstractNode::run but set for node custom work folder.
     * This meaby use for multiple deployment nodes on one host.
     * @param addres This is network address of work node or server.
     *  If address is empty then server weel be listen all addreses of all interfaces else listen only selected address.
     * @param port This is port of deployment node (server)
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

protected:

    /**
     * @brief initDefaultDbObjects This method create a default cache and database writer objects if the input pointers is null
     *  Override this method for create a custom database objects for your node class.
     * @note If you override this object then you no longer need to overload the run method to set your own controls.
     * This method invoked automaticlhy when ypu call the DataBaseNode::run method.
     * @param cache This is Cache database object.
     * @param writer This is Database writerObject.
     */
    virtual void initDefaultDbObjects(SqlDBCache *cache, SqlDBWriter *writer);

    ParserResult parsePackage(const Package &pkg,
                              const AbstractNodeInfo* sender) override;

    AbstractNodeInfo *createNodeInfo(QAbstractSocket *socket, const HostAddress *clientAddress) const override;

    void badRequest(const HostAddress &address, const Header &req,
                    const PKG::ErrorData& err, qint8 diff = REQUEST_ERROR) override;

    bool changeTrust(const HostAddress &id, int diff) override;

    /**
     * @brief changeTrust This implementation of change trust is change trust node or user by self id.
     * All changes of trust saving into local database.
     * @param id This is id of user of other network member object.
     * @param diff This is dfference of trust.
     * @return true if trust of user changed successful.
     */
    virtual bool changeTrust(const QVariant &id, int diff);

    /**
     * @brief sendData This method is some as AbstractNode::sendData but try send data to the id.
     *  This implementation prepare object to sending.
     * @param resp This is sending object to the nodeId.
     * @param nodeId This is id of target node.
     * @param req This is header of request.
     * @return true if data sendet seccussful
     */
    virtual bool sendData(PKG::AbstractData *resp, const QVariant &nodeId,
                          const Header *req = nullptr);

    /**
     * @brief sendData This method is some as AbstractNode::sendData but try send data to the id.
     *  This implementation do not prepare object to sending.
     * @param resp This is sending object to the nodeId.
     * @param nodeId This is id of target node.
     * @param req This is header of request.
     * @return true if data sendet seccussful
     */
    virtual bool sendData(const PKG::AbstractData *resp, const QVariant &nodeId,
                          const Header *req = nullptr);

    bool sendData(const PKG::AbstractData *resp, const HostAddress &nodeId,
                  const Header *req = nullptr) override;
    bool sendData(PKG::AbstractData *resp, const HostAddress &nodeId,
                  const Header *req = nullptr) override;

    /**
     * @brief hashgenerator This method generate a hash from any value.
     * Override this method for set your custom salt.
     * @param data This input byte array
     */
    virtual QByteArray hashgenerator(const QByteArray &data);

    void nodeConnected(const HostAddress &node) override;

    /**
     * @brief db this node return pointer to database object.
     * @return the pinter to data base
     */
    SqlDBCache* db() const;

    /**
     * @brief deleteObject This method delete object by database address.
     *  @note If you want to delete any object use only this method becouse this method check permision of requester to execute this action
     * @param requester This is pointer to network meber that send this request.
     * @param dbObject This is pointer to object of database to remove.
     * @return result of operation (allow, forbiden unknown).
     *  For more information about results see the DBOperationResult enum.
     */
    DBOperationResult deleteObject(const QVariant &requester,
                                   const QSharedPointer<PKG::DBObject> &dbObject);

    /**
     * @brief getObject This method try get an object by database address.
     *  @note If yoo want to get any object use only this method becouse this method check permision of requester to execute this action
     * @param requester This is pointer to network meber that send this request.
     * @param templateObj This is pointer to object of database with data for generation the sql select request.
     * @param result This is a shared pointer for save result of request.
     * @return result of operation (allow, forbiden unknown).
     *  For more information about results see the DBOperationResult enum.
     */
    DBOperationResult getObject(const QVariant &requester,
                                const PKG::DBObject &templateObj,
                                QSharedPointer<PKG::DBObject> &result) const;

    /**
     * @brief getObjects This method try get objects by template object.
     *  @note If yoo want to get any objects use only this method becouse this method check permision of requester to execute this action
     * @param requester This is pointer to network meber that send this request.
     * @param templateObj This is pointer to object of database with data for generation the sql select request.
     * @param result This is reference to the list of result objects.
     * @return result of operation (allow, forbiden unknown).
     *  For more information about results see the DBOperationResult enum.
     */
    DBOperationResult getObjects(const QVariant &requester,
                                 const PKG::DBObject &templateObj,
                                 QList<QSharedPointer<PKG::DBObject>> &result) const;

    /**
     * @brief updateObject This method try save or update database object.
     *  @note If yoo want to save or update any objects use only this method becouse this method check permision of requester to execute this action
     * @param requester This is network meber that send this request.
     * @param saveObject This is pointer to object of database for save or update.
     * @return result of operation (allow, forbiden unknown).
     *  For more information about results see the DBOperationResult enum.
     */
    DBOperationResult updateObject(const QVariant &requester,
                                const QSharedPointer<PKG::DBObject> &saveObject);

    /**
     * @brief createObject This method create a new object in the database and add all permisions forthe objects creator.
     *  @note If you want to create any objects use only this method becouse this method check permision of requester to execute this action
     * @param requester This is network meber that send this request.
     * @param obj This is pointer to object of database for save or update.
     * @return result of operation (allow, forbiden unknown).
     *  For more information about results see the DBOperationResult enum.
     */
    DBOperationResult createObject(const QVariant &requester,
                                    const QSharedPointer<PKG::DBObject> &obj);

    /**
     * @brief updateIfNotExistsCreateObject This is wraper of the updateObject and createObjects methods.
     * 1. try update object
     * 2. If object not exists Try create new object.
     * 3. return operation result
     * @param requester This is network meber that send this request.
     * @param obj This is initialising object.
     * @return result of operation (allow, forbiden unknown).
     *  For more information about results see the DBOperationResult enum.
     */
    DBOperationResult updateIfNotExistsCreateObject(const QVariant &requester,
                                                    const QSharedPointer<PKG::DBObject> &obj);

    /**
     * @brief changeObjects This is wrapper of the "ISqlDBCache::changeObjects" method.
     * 0 Kye difference between a base method is checking of the permision for needed action.
     * @note If you want to change any objects use only this method becouse this method check permision of requester to execute this action
     * @param requester This is network meber that send this request.
     * @param templateObj This is pointer to object of database with data for generation the sql select request.
     * @param changeAction This is action funnction for change all selected objects.
     * @return result of operation (allow, forbiden unknown).
     *  For more information about results see the DBOperationResult enum.
     */
    DBOperationResult changeObjects(const QVariant &requester,
                                    const PKG::DBObject &templateObj,
                                    const std::function<bool (const QSharedPointer<QH::PKG::DBObject>&)> &changeAction);


    /**
     * @brief getSender This method return id of requester.
     *  By Default base implementation get id from BaseNdoeInfo.
     * override this method for correctly work of the DataBaseNode::ParsePacakge method.
     * @param connectInfo This is info about connection.
     * @param requestData This is data of request.
     * @return id of requester member.
     */
    virtual const QVariant *getSender(const AbstractNodeInfo *connectInfo, const PKG::AbstractData *requestData) const;

    /**
     * @brief checkPermision This method check a permision of requester to database object with objectAddress.
     *  Override this method if ypu have a custom database structure.
     * @param requester This is  user, node or another object of network
     * @param objectAddress This is addres to database object
     * @param requarimentPermision This is needed permision for requester
     * @return DBOperationResult::Alowed if permission granted.
     *  For m more information about result see the DBOperationResult enum.
     */
    virtual DBOperationResult checkPermission(const QVariant &requester,
                                const DbAddress& objectAddress,
                                const Permission& requarimentPermision) const;


    /**
     * @brief addUpdatePermission This method added or update permision for member.
     * @warning This method do not have a validation. It is just change a NetworkMembers table, so use this carefully.
     * @param member This is member id (user of node)
     * @param objectAddress This is database  object for which the permissions will be set
     * @param permision This is Permision level
     * @return true if method finished successful.
     */
    virtual bool addUpdatePermission(const QVariant &member,
                                     const DbAddress& objectAddress,
                                     const Permission& permision) const;


    /**
     * @brief removePermission This method removed permision for member.
     * @warning This method do not have a validation. It is just change a NetworkMembers table, so use this carefully.
     * @param member This is member id (user of node)
     * @param objectAddress This is database  object for which the permissions will be removed
     * @return true if method finished successful.
     */
    virtual bool removePermission(const QVariant &member,
                                  const DbAddress& objectAddress) const;

    /**
     * @brief dbLocation This method return location of nodes or clients database.
     * @return path to the location of database
     */
    QString dbLocation() const;

    /**
     * @brief welcomeAddress This method send to the ip information about yaster self.
     * Override this method if you want send custom data to incomming connection.
     * @param ip This is host address of the peer node obeject
     * @return true if all iformation sendet succesful
     */
    virtual bool welcomeAddress(const HostAddress &ip);

    /**
     * @brief isBanned This method check trust of node, if node trus is lover of 0 return true.
     * @param member This is member of network (node, client or server)
     * @return true if node is banned
     */
    bool isBanned(const QVariant &member) const;

    /**
     * @brief SQLSources This method contains list of sqldatabase sources.
     * This method will be invoced into initialize sql method and deploy sql database.
     *
     * All sql files will be deployed in QList order.
     *
     *  By Default This metod deploy next sql code:
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
     * For add own sql code hust override this method, but do not forgat invoke a base method of a parent class.
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
     * @brief systemTables This method return the set of tables that forbidet for users.
     * By default is NetworkMembers and MemberPermisions tables.
     * @return set of tables names.
     */
     virtual QSet<QString> systemTables() const;
private:

    /**
     * @brief workWithSubscribe This metod work work with subscribe commnads
     * @param rec This is request data
     * @param address This is sendet address
     * @return true if data parsed seccusseful
     */
    bool workWithSubscribe(const PKG::WebSocket &rec,
                           const QVariant &clientOrNodeid,
                           const AbstractNodeInfo &sender);


    bool isForbidenTable(const QString& table);

    SqlDBCache *_db = nullptr;
    QString _localNodeName;
    WebSocketController *_webSocketWorker = nullptr;


    friend class WebSocketController;

};


}
#endif // DATABASENODE_H
