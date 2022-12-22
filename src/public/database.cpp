/*
 * Copyright (C) 2018-2022 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
*/

#include "database.h"
#include "sqldbwriter.h"
#include "asyncsqldbwriter.h"

#include <quasarapp.h>
#include <QCoreApplication>
#include <deleteobject.h>
#include <QSet>
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
                         ISqlDB *cache,
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

    connect(_db, &ISqlDB::sigItemChanged,
            this, &DataBase::sigObjectChanged,
            Qt::DirectConnection);

    connect(_db, &ISqlDB::sigItemDeleted,
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

void DataBase::initDefaultDbObjects(ISqlDB *cache,
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

const DBPatchMap DataBase::dbPatches() const {
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

QString DataBase::dbLocation() const {
    if (db() && db()->writer()) {
        return db()->writer()->databaseLocation();
    }

    return "";
}

ISqlDB *DataBase::db() const {
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

    if (currentVersion < _targetDBVersion)
        onBeforeDBUpgrade(currentVersion, _targetDBVersion);

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

        if (!_db->replaceObject(updateVersionRequest, true)) {
            QuasarAppUtils::Params::log("Failed to update version attribute",
                                        QuasarAppUtils::Error);
            return false;
        }
    }

    return true;
}

void DataBase::onBeforeDBUpgrade(int , int ) const { }

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
}

