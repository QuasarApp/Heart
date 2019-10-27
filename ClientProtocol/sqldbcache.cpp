#include "sqldbcache.h"
#include "quasarapp.h"
#include "sqldbwriter.h"

#include <clientprotocol.h>
#include <dbobject.h>

#include <QDateTime>
#include <QtConcurrent/QtConcurrent>


namespace ClientProtocol {

void SqlDBCache::globalUpdateDataBasePrivate(qint64 currentTime) {
//    for (auto item = items.begin(); item != items.end(); ++item) {
//        if (SqlDBWriter::saveItem(item.value()) < 0) {
//            QuasarAppUtils::Params::verboseLog("writeUpdateItemIntoDB failed when"
//                                               " work globalUpdateDataRelease!!! id=" +
//                                                QString::number(item.key()),
//                                               QuasarAppUtils::VerboseLvl::Error);
//        }
//    }

//    for (auto player = players.begin(); player != players.end(); ++player) {
//        if (SqlDBWriter::savePlayer(player.value()) < 0) {
//            QuasarAppUtils::Params::verboseLog("writeUpdatePlayerIntoDB failed when"
//                                               " work globalUpdateDataRelease!!! id=" +
//                                                QString::number(player.key()),
//                                               QuasarAppUtils::VerboseLvl::Error);

//        }
//    }

//    for (auto owner = owners.begin(); owner != owners.end(); ++owner) {
//        if (!SqlDBWriter::saveowners(owner.key(), owner.value())) {
//            QuasarAppUtils::Params::verboseLog("UpdateInfoOfowners failed when"
//                                               " work globalUpdateDataRelease!!! id=" +
//                                                QString::number(owner.key()),
//                                               QuasarAppUtils::VerboseLvl::Error);
//        }
//    }

//    lastUpdateTime = currentTime;
}

void SqlDBCache::globalUpdateDataBase(SqlDBCasheWriteMode mode) {
    qint64 currentTime = QDateTime::currentMSecsSinceEpoch();

    if (currentTime - lastUpdateTime > updateInterval ||
            static_cast<bool>(mode & SqlDBCasheWriteMode::Force)) {

        if (static_cast<bool>(mode & SqlDBCasheWriteMode::On_New_Thread)) {

            QtConcurrent::run([currentTime, this](){
                globalUpdateDataBasePrivate(currentTime);
            });

        } else {
            globalUpdateDataBasePrivate(currentTime);
        }
    }
}

SqlDBCache::SqlDBCache(qint64 updateInterval) {
    lastUpdateTime = QDateTime::currentMSecsSinceEpoch();
    this->updateInterval = updateInterval;
}

SqlDBCache::~SqlDBCache() {
    globalUpdateDataBase(SqlDBCasheWriteMode::Force);
}

SqlDBWriter *SqlDBCache::writer() const {
    return _writer;
}

void SqlDBCache::setWriter(SqlDBWriter *writer) {
    _writer = writer;
}

bool SqlDBCache::getObject(DBObject *result, const QString &table, int id) {

    if (!_writer) {
        return false;
    }

    auto& tableObj = _cache[table];

    if (!_cache.contains(table)) {

    }

    if ()

    return true;
}

bool SqlDBCache::saveObject(DBObject *saveObject) {

}

bool SqlDBCache::deleteObject(const QString &table, int id) {

}

bool SqlDBCache::init(const QString &initDbParams) {

    if (!_writer) {
        return false;
    }

    if (!_writer->initDb(initDbParams)) {
        return false;
    }
}

}
