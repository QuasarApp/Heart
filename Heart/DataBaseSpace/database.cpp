/*
 * Copyright (C) 2018-2022 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
*/

#include "accesstoken.h"
#include "database.h"
#include "sqldbcache.h"
#include "sqldbwriter.h"
#include "asyncsqldbwriter.h"

#include <quasarapp.h>
#include <QCoreApplication>
#include <basenodeinfo.h>
#include <abstractnetworkmember.h>
#include <memberpermisionobject.h>
#include <networkmember.h>
#include <deleteobject.h>
#include <QSet>
#include <defaultpermision.h>
#include <itoken.h>
#include <sqlitedbcache.h>
#include <sqldb.h>
#include "getsinglevalue.h"
#include "setsinglevalue.h"
#include <qaglobalutils.h>

namespace QH {
using namespace PKG;

DataBase::DataBase(QObject *ptr): QObject(ptr) {
    qRegisterMetaType<QSharedPointer<QH::PKG::DBObject>>();
}

bool DataBase::initSqlDb(QString DBparamsFile,
                         ISqlDBCache *cache,
                         SqlDBWriter *writer) {

    initDefaultDbObjects(cache, writer);

    QVariantMap params;
    _db->setSQLSources(SQLSources());

    if (DBparamsFile.isEmpty()) {
        params = defaultDbParams();

        if (!_db->init(params)) {
            return false;
        }

    } else {

        if (!_db->init(DBparamsFile)) {
            return false;
        }

    }

    if (!upgradeDataBase()) {
        QuasarAppUtils::Params::log("Failed to upgrade database",
                                    QuasarAppUtils::Error);
        return false;
    }

    QuasarAppUtils::Params::log(QString("Database loaded from: %0").arg(dbLocation()),
                                QuasarAppUtils::Debug);

    connect(_db, &ISqlDBCache::sigItemChanged,
            this, &DataBase::sigObjectChanged,
            Qt::DirectConnection);

    connect(_db, &ISqlDBCache::sigItemDeleted,
            this, &DataBase::sigObjectDeleted,
            Qt::DirectConnection);

    return true;
}

bool DataBase::isSqlInited() const {
    return _db;
}

bool DataBase::run() {
    if (!isSqlInited() && !initSqlDb()) {
        return false;
    }
    return true;
}

bool DataBase::run(const QString &localNodeName) {

    if (localNodeName.isEmpty())
        return false;

    setLocalNodeName(localNodeName);

    if (!isSqlInited() && !initSqlDb()) {
        return false;
    }

    return true;
}

void DataBase::stop() {

    if (db()) {
        auto writer = _db->writer();
        _db->softDelete();
        _db = nullptr;
        delete writer;
    }
}

DataBase::~DataBase() {
    stop();
}

void DataBase::initDefaultDbObjects(ISqlDBCache *cache,
                                    SqlDBWriter *writer) {
    if (!writer) {
        writer = new AsyncSqlDBWriter();
    }

    if (!cache) {
        cache = new SqlDB();
    }

    cache->setWriter(writer);
    _db = cache;
}


bool DataBase::welcomeAddress(AbstractNodeInfo *) {
    return true;
}

bool DataBase::isBanned(const QString &node) const {
    NetworkMember member(node);
    auto objectFromDataBase = db()->getObject<AbstractNetworkMember>(member);

    if (!objectFromDataBase)
        return false;

    return objectFromDataBase->trust() <= 0;
}

QStringList DataBase::SQLSources() const{
    return {
        DEFAULT_DB_INIT_FILE_PATH
    };
}

QSet<QString> DataBase::systemTables() const {
    return {"NetworkMembers", "MemberPermisions"};
}

void DataBase::objectRemoved(const DbAddress &) {

}

void DataBase::objectChanged(const QSharedPointer<DBObject> &) {

}

const DBPatchMap &DataBase::dbPatches() const {
    return _dbPatches;
}

void DataBase::addDBPatch(const DBPatch &patch) {
    debug_assert(patch.isValid(),
                 "Failed to initialise a Data base patch!"
                 " Patch object is invalid");


    debug_assert(!_dbPatches[patch.versionFrom].contains(patch.versionTo),
                 "Failed to initialise a Data base patch!");

    _dbPatches[patch.versionFrom][patch.versionTo] = patch;

    _targetDBVersion = std::max(_targetDBVersion, patch.versionTo);
}

void DataBase::memberSubsribed(const QVariant &, unsigned int ) {
    return;
}

void DataBase::memberUnsubsribed(const QVariant &, unsigned int ) {
    return;
}

QString DataBase::dbLocation() const {
    if (db() && db()->writer()) {
        return db()->writer()->databaseLocation();
    }

    return "";
}

bool DataBase::changeTrust(const QString &id, int diff) {
    if (!_db)
        return false;

    auto action = [diff](const QSharedPointer<DBObject> &object) {
        auto obj = object.dynamicCast<AbstractNetworkMember>();
        if (!obj) {
            return false;
        }

        obj->changeTrust(diff);

        return true;
    };

    return _db->changeObjects(NetworkMember{id}, action);
}

ISqlDBCache *DataBase::db() const {
    return _db;
}

bool DataBase::isForbidenTable(const QString &table) {
    return systemTables().contains(table);
}

bool DataBase::upgradeDataBase() {
    if (!db())
        return false;

    DBPatchMap patchesPack = dbPatches();
    if (!patchesPack.size()) {
        return true;
    }

    int currentVersion = 0;

    bool fsupportUpgrade = db()->doQuery("SELECT COUNT(*) FROM DataBaseAttributes", true);

    if (!fsupportUpgrade) {

        QuasarAppUtils::Params::log("The data base of application do not support soft upgrade. "
                                    "Please remove database monyaly and restart application."
                                    "You can disable upgrade functions for this override the upgradeDataBase method. ",
                                    QuasarAppUtils::Error);
        return false;
    }

    PKG::GetSingleValue request({"DataBaseAttributes", "version"}, "value", "name");

    if (auto responce = _db->getObject(request)) {
        currentVersion = responce->value().toInt();
    }    

    while (currentVersion < _targetDBVersion) {

        QString message;
        message = "Upgrade data base from %0 to %1 versions. %2";
        message = message.arg(currentVersion);

        auto patches = patchesPack.value(currentVersion, {});

        if (!patches.size()) {
            QuasarAppUtils::Params::log("Failed to " + message.arg("Unknown", "Required patch not found!"),
                                        QuasarAppUtils::Error);
            return false;
        }

        auto patch = patches.last();
        message = message.arg(patch.versionTo);

        QuasarAppUtils::Params::log(message.arg("(Begin)"),
                                    QuasarAppUtils::Info);

        if (!patch.action(db())) {
            QuasarAppUtils::Params::log("Failed to " + message.arg("Patch finished with error code!"),
                                        QuasarAppUtils::Error);
            return false;
        }

        currentVersion = patch.versionTo;

        auto updateVersionRequest = QSharedPointer<PKG::SetSingleValue>::create(
                    DbAddress{"DataBaseAttributes", "version"},
                    "value", currentVersion, "name");

        if (!_db->insertIfExistsUpdateObject(updateVersionRequest, true)) {
            QuasarAppUtils::Params::log("Failed to update version attribute",
                                        QuasarAppUtils::Error);
            return false;
        }
    }

    return true;
}

const QString &DataBase::localNodeName() const {
    return _localNodeName;
}

void DataBase::setLocalNodeName(const QString &newLocalNodeName) {
    _localNodeName = newLocalNodeName;
}

QVariantMap DataBase::defaultDbParams() const {

    return {
        {"DBDriver", "QSQLITE"},
        {"DBFilePath", DEFAULT_DB_PATH + "/" + localNodeName() + "/" + localNodeName() + "_" + DEFAULT_DB_NAME},
    };
}

DBOperationResult
QH::DataBase::getObject(const QString &requester,
                        const QH::DBObject &templateObj,
                        QSharedPointer<QH::PKG::DBObject> &result) const {

    if (!(_db && result)) {
        return DBOperationResult::Unknown;
    }

    DBOperationResult permisionResult = checkPermission(requester, templateObj.dbAddress(),
                                                        Permission::Read);
    if (permisionResult != DBOperationResult::Allowed) {
        return permisionResult;
    }

    auto obj = _db->getObject(templateObj);
    if (!obj || (obj->dbAddress() != templateObj.dbAddress())) {
        return DBOperationResult::Unknown;
    }

    result = obj;
    return DBOperationResult::Allowed;
}

DBOperationResult
DataBase::getObjects(const QString &requester,
                     const DBObject &templateObj,
                     QList<QSharedPointer<DBObject>> &result) const {
    if (!_db) {
        return DBOperationResult::Unknown;
    }

    if (!_db->getAllObjects(templateObj, result)) {
        return DBOperationResult::Unknown;
    }

    for (const auto& obj: qAsConst(result)) {
        if (!obj)
            return DBOperationResult::Unknown;

        auto permisionResult = checkPermission(requester, obj->dbAddress(),
                                               Permission::Read);
        if (permisionResult != DBOperationResult::Allowed) {
            return permisionResult;
        }
    }

    return DBOperationResult::Allowed;
}

DBOperationResult
DataBase::updateObject(const QString &requester,
                       const QSharedPointer<DBObject> &saveObject) {

    if (!_db) {
        return DBOperationResult::Unknown;
    }

    auto permisionResult = checkPermission(requester,
                                           saveObject->dbAddress(),
                                           Permission::Write);
    if (permisionResult != DBOperationResult::Allowed) {
        return permisionResult;
    }

    if (!_db->updateObject(saveObject)) {
        return DBOperationResult::Unknown;
    }

    return DBOperationResult::Allowed;
}

DBOperationResult
DataBase::createObject(const QString &requester,
                       const QSharedPointer<DBObject> &obj) {

    if (!_db) {
        return DBOperationResult::Unknown;
    }

    if (isForbidenTable(obj->table())) {
        return DBOperationResult::Forbidden;
    }

    if (!_db->insertObject(obj)) {
        return DBOperationResult::Unknown;
    }

    if (!addUpdatePermission(requester, obj->dbAddress(), Permission::Write)) {

        _db->deleteObject(obj);

        return DBOperationResult::Forbidden;
    }

    return DBOperationResult::Allowed;

}

DBOperationResult
DataBase::updateIfNotExistsCreateObject(const QString &requester,
                                        const QSharedPointer<DBObject> &obj) {

    auto opResult = updateObject(requester, obj);
    if (opResult != QH::DBOperationResult::Unknown) {
        return opResult;
    }

    return createObject(requester, obj);
}

DBOperationResult
DataBase::changeObjects(const QString &requester,
                        const DBObject &templateObj,
                        const std::function<bool (const QSharedPointer<DBObject> &)> &changeAction) {

    DBOperationResult result = DBOperationResult::Unknown;

    if (!_db) {
        return result;
    }

    auto execWithCheck = [this, requester, &result, &changeAction]
            (const QSharedPointer<DBObject> & obj) {

        result = checkPermission(requester, obj->dbAddress(), Permission::Update);
        if (result != DBOperationResult::Allowed) {
            return false;
        }

        return changeAction(obj);
    };

    if (!db()->changeObjects(templateObj, execWithCheck)) {
        return result;
    }

    return result;
}

QVariant DataBase::getSender(const AbstractNodeInfo *connectInfo,
                             const AbstractData *) const {

    auto info = dynamic_cast<const BaseNodeInfo*>(connectInfo);
    if (!info)
        return {};

    return info->id();
}

DBOperationResult
DataBase::checkPermission(const QString &requester,
                          const DbAddress &objectAddress,
                          const Permission& requarimentPermision) const {

    if (!requester.isEmpty())
        return DBOperationResult::Unknown;

    if (!_db) {
        return DBOperationResult::Unknown;
    }

    auto member = _db->getObjectRaw(NetworkMember{requester});
    if (!member) {
        return DBOperationResult::Unknown;
    }

    auto permision =
            _db->getObject(MemberPermisionObject({requester, objectAddress}));

    if (!permision) {

        permision = _db->getObject(DefaultPermision({requester, objectAddress}));
        if (!permision)
            return DBOperationResult::Unknown;
    }

    if (permision->permisions() < requarimentPermision) {
        return DBOperationResult::Forbidden;
    }

    return DBOperationResult::Allowed;
}

bool DataBase::addUpdatePermission(const QVariant &member,
                                   const DbAddress &objectAddress,
                                   const Permission &permision,
                                   const Permission &defaultPermision) const {

    if (!_db) {
        return false;
    }

    auto object = QSharedPointer<MemberPermisionObject>::create();
    object->setKey(PermisionData(member, objectAddress));
    object->setPermisions(permision);

    if (!_db->insertObject(object) && !_db->updateObject(object)) {
        return false;
    }

    auto defaultPermisionObject = QSharedPointer<DefaultPermision>::create();
    defaultPermisionObject->setKey(PermisionData({}, objectAddress));
    defaultPermisionObject->setPermisions(defaultPermision);

    if (!_db->insertObject(defaultPermisionObject) &&
            !_db->updateObject(defaultPermisionObject)) {
        return false;
    }

    return true;
}

bool DataBase::removePermission(const QVariant &member,
                                const DbAddress &objectAddress) const {

    if (!_db) {
        return false;
    }

    auto object = QSharedPointer<MemberPermisionObject>::create();
    object->setKey(PermisionData(member, objectAddress));

    if (!_db->deleteObject(object)) {
        return false;
    }

    return true;
}

DBOperationResult
DataBase::deleteObject(const QString &requester,
                       const QSharedPointer<DBObject> &dbObject) {

    if (!_db) {
        return DBOperationResult::Unknown;
    }

    auto permisionResult = checkPermission(requester,
                                           dbObject->dbAddress(),
                                           Permission::Write);
    if (permisionResult != DBOperationResult::Allowed) {
        return permisionResult;
    }

    auto address = dbObject->dbAddress();
    if (!_db->deleteObject(dbObject)) {
        return DBOperationResult::Unknown;
    }

    return DBOperationResult::Allowed;
}

}

