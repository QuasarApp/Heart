/*
 * Copyright (C) 2018-2023 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
*/

#ifndef QH_DATABASE_H
#define QH_DATABASE_H

#include "dbobjectsrequest.h"
#include "dbpatch.h"
#include "isqldb.h"
#include <dbobject.h>
#include <hostaddress.h>


namespace QH {

namespace PKG {
class WebSocket;
class ISubscribableData;
}

class ISqlDB;
class SqlDBWriter;
class DbAddress;
class NodeId;
class iObjectProvider;
class AbstractNodeInfo;

/**
 * @brief The DataBase class is DataBase base implementation.
 *  This implementation contains methods for work with database.
 *  DataBaseNode is thread save class.
 *
 *  @note before use the database you need to run it. Use The DataBase::run method for this. If you need to stop database, run the DataBase::stop method.

 * @see DBObject
 * @see DataBaseNode
 */
class HEARTSHARED_EXPORT DataBase: public QObject
{
    Q_OBJECT
public:

    DataBase(QObject * ptr = nullptr);
    ~DataBase();

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
                            ISqlDB * cache = nullptr,
                            SqlDBWriter* writer = nullptr);

    /**
     * @brief isSqlInited This method return true if database initialized successful.
     * @return True if database initialized successful.
     */
    bool isSqlInited() const;

    /**
     * @brief run This method start and initialize the data base connection.
     * @return true if finished successful else false.
     */
    bool run();

    /**
     * @brief run This method is some as AbstractNode::run but set for node custom work folder.
     * This maybe use for multiple deployment nodes on one host.
     * @param localNodeName This is name of node and  work folder of node.
     */
    virtual bool run(const QString &localNodeName);

    /**
     * @brief stop This method stop and diskonnect the database.
     */
    void stop();

    /**
     * @brief defaultDbParams This method return default database parameters for this node.
     * Override this method for set new default parameters. Or use json database parameters file in DataBaseNode::run.
     * @return Map with all database parameters.
     *  For more information of available parameters see the SqlDBWriter::defaultInitPararm method.
     */
    virtual QVariantMap defaultDbParams() const;

    /**
     * @brief dbLocation This method return location of nodes or clients database.
     * @return path to the location of database.
     */
    QString dbLocation() const;

signals:

    /**
     * @brief sigItemChanged This signal emitted when database object is changed.
     * @note emit implemented in updateObject and insertObject methods.
     *  So If you override then methods do not forget add emit of the sigItemChanged signal.
     * @param obj This is changed object.
     * @note This is wrapper of the ISqlDBCache::sigItemChanged
     */
    void sigObjectChanged(const QSharedPointer<QH::PKG::DBObject> &obj);

    /**
     * @brief sigItemDeleted This signal emitted when database object is deleted.
     * @note emit implemented in the deleteObject method.
     *  So if you override the deleteObject method do not forget add emit of the sigItemChanged signal.
     * @param obj This is address of the removed object.
     * @note This is wrapper of the ISqlDBCache::sigItemDeleted
     */
    void sigObjectDeleted(const QH::DbAddress& obj);

protected:

    /**
     * @brief backUp This method make a backup of database.
     * @param version This is current version of db.
     * @return path to backupped db.
     */
    QString backUp(int version) const;

    /**
     * @brief localNodeName This method return local node name.
     * @return local node name
     */
    const QString &localNodeName() const;

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
    virtual void initDefaultDbObjects(ISqlDB *cache, SqlDBWriter *writer);

    /**
     * @brief db This node return pointer to database object.
     * @return The pointer to data base.
     */
    ISqlDB* db() const;

    /**
     * @brief welcomeAddress This method send to the node information about self.
     * Override this method if you want send custom data to incoming connection.
     * @param node This is info object of the peer node.
     * @return true if all information sender successful.
     */
    virtual bool welcomeAddress(AbstractNodeInfo *node);

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
        addDBPatch({
                       0, // fromVersion
                       1, // toVersion
                       [](const QH::iObjectProvider* database) -> bool {
                            // Some code for update from 0 to 1
                       } // action of patch
                   });

        addDBPatch({
                       1, // fromVersion
                       2, // toVersion
                       [](const QH::iObjectProvider* database) -> bool {
                            // Some code for update from 1 to 2
                       } // action of patch
                   });

        addDBPatch({
                       2, // fromVersion
                       3, // toVersion
                       [](const QH::iObjectProvider* database) -> bool {
                            // Some code for update from 2 to 3
                       } // action of patch
                   });
     *  @endcode
     *
     * @return Map of database pactches.
     *
     * @see DBPatchMap
     * @see DBPatch
     * @see DataBase::addDBPatch
     * @see DataBase::onBeforeDBUpgrade
     */
    virtual const DBPatchMap dbPatches() const;

    /**
     * @brief addDBPatch This method add database patch to the data base object.
     * @param patch This is object of the database patch
     * @note This method will be crashed if patch is invalid.
     * @see DataBase::dbPatches
     * @see DataBase::onBeforeDBUpgrade
     */
    void addDBPatch(const DBPatch& patch);

    /**
     * @brief upgradeDataBase This method upgrade data base to actyaly database version.
     * @note The last version of dbPatches is actyaly version.
     * @return true if operation finished successful
     * @note if you want to disable this feature then override this method and return true.
     */
    virtual bool upgradeDataBase();

    /**
     * @brief onBeforeDBUpgrade This method will be invoked before upgrade database.
     * @param currentVerion This is current database version
     * @param tergetVersion This is target database version.
     * @see DataBase::dbPatches
     * @see DataBase::addDBPatch
     */
    virtual void onBeforeDBUpgrade(int currentVerion, int tergetVersion) const;


    /**
     * @brief Get an object by its identifier.
     *
     * This method retrieves an object of type @c Object from the database using the
     * given identifier and setter function.
     *
     * @tparam Object The type of object to retrieve.
     * @tparam Id The type of the identifier.
     * @tparam Setter The type of the setter function.
     * @param id The identifier of the object.
     * @param setter The setter function to set the identifier in the object.
     * @return A pointer to an object of type QSharedPointer<Object>, or nullptr if the object is not found.
     *
     * Example:
     *
     * @code{cpp}
     *          return getById<MenuItem>(id, &MenuItem::setId);
     * @endcode
     */
    template <class Object, class Id, class Setter>
    QSharedPointer<Object> getById(const Id& id, Setter setter) {
        if (auto&& database = db()) {
            Object request;
            (request.*setter)(id);
            return database->getObject(request);
        }

        return nullptr;
    };

    /**
     * @brief Delete an object by its identifier.
     *
     * This method deletes an object of type @c Object from the database using the
     * given identifier and setter function.
     *
     * @tparam Object The type of object to delete.
     * @tparam Id The type of the identifier.
     * @tparam Setter The type of the setter function.
     * @param id The identifier of the object.
     * @param setter The setter function to set the identifier in the object.
     * @return true if the object is successfully deleted, false otherwise.
     *
     * Example:
     *
     * @code{cpp}
            deleteById<Role>(userId, &Role::setUserId);
     * @endcode
     */
    template <class Object, class Id, class Setter>
    bool deleteById(const Id& id, Setter setter) {
        if (auto&& database = db()) {
            auto&& request = QSharedPointer<Object>::create();
            (*request.*setter)(id);
            return database->deleteObject(request);
        }

        return false;
    };

    /**
     * @brief Save an object in the database.
     *
     * This method saves an object of type @c Object in the database.
     *
     * @tparam Object The type of object to save.
     * @param obj The object to save.
     * @return true if the object is successfully saved, false otherwise.
     *
     * Example:
     *
     * @code{cpp}
            saveObj(role.dynamicCast<Role>());
     * @endcode
     */
    template <class Object>
    bool saveObj(const Object& obj) {
        if (auto&& database = db()) {
            return database->replaceObject(obj);
        }

        return false;
    };

    /**
     * @brief Get a list of all objects from a specified table.
     *
     * This method retrieves a list of all objects of type @c Object from the specified
     * table in the database.
     *
     * @tparam Object The type of objects to retrieve.
     * @param table The name of the table in the database.
     * @return A list of pointers to objects of type QSharedPointer<Object>.
     * If no objects are found, an empty list is returned.
     *
     * Example:
     *
     * @code{cpp}
            auto&& data = getAll<User>("Users");
     * @endcode
     */
    template<class Object>
    QList<QSharedPointer<Object>> getAll(const QString& table) {
        QH::PKG::DBObjectsRequest<Object> request(table);

        auto&& response = db()->getObject(request);
        if (!response) {
            return {};
        }

        return response->data();
    }

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

    ISqlDB *_db = nullptr;
    unsigned short _targetDBVersion = 0;
    DBPatchMap _dbPatches;
    QString _localNodeName;
    friend class DataBaseNode;

};


}

Q_DECLARE_METATYPE(QSharedPointer<QH::PKG::DBObject>)
#endif // QH_DATABASE_H
