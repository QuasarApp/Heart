#include "sqldbcache.h"
#include "quasarapp.h"
#include "playerdbdata.h"
#include <qtconcurrentrun.h>
#include <clientprotocol.h>

#include <QDateTime>
#include <basenetworkobject.h>

int SqlDBCache::generateIdForItem() const {
    if (items.isEmpty()) {
        return 0;
    }

    return items.lastKey() + 1;
}

int SqlDBCache::generateIdForPalyer() const {
    if (players.isEmpty()) {
        return 0;
    }

    return players.lastKey() + 1;
}

bool SqlDBCache::checkPlayer(int id) {
    if (players.contains(id)) {
        return true;
    }

    if (SqlDBWriter::checkPlayer(id)) {

        if (savePlayer(getPlayer(id)) < 0) {
            QuasarAppUtils::Params::verboseLog("not saved data into cache "
                                               " SqlDBCashe::checkPlayer");
        }

        return true;
    }

    return false;
}

bool SqlDBCache::checkItem(int idItem, int idOwner) {

    if (idOwner >= 0 ) {

        if (owners.contains(idOwner)) {
            auto items = owners.value(idOwner);
            return items.contains(idItem);
        }

        if (SqlDBWriter::checkItem(idItem)) {
            QSet<int> items;
            if (!SqlDBWriter::getAllItemsOfPalyer(idOwner, items)) {
                QuasarAppUtils::Params::verboseLog("not loaded owners data from cache "
                                                   " SqlDBCashe::checkItem");
            }

            owners.insert(idOwner, items);

            return true;
        }

        return false;

    }

    if (items.contains(idItem)) {
        return true;
    }

    if (SqlDBWriter::checkItem(idItem)) {

        auto item = getItem(idItem);
        if (saveItem(item) < 0) {
            QuasarAppUtils::Params::verboseLog("not saved data into cache "
                                               " SqlDBCashe::checkItem");
        }

        return true;
    }

    return false;
}

void SqlDBCache::globalUpdateDataBasePrivate(qint64 currentTime) {
    for (auto item = items.begin(); item != items.end(); ++item) {
        if (SqlDBWriter::saveItem(item.value()) < 0) {
            QuasarAppUtils::Params::verboseLog("writeUpdateItemIntoDB failed when"
                                               " work globalUpdateDataRelease!!! id=" +
                                                QString::number(item.key()),
                                               QuasarAppUtils::VerboseLvl::Error);
        }
    }

    for (auto player = players.begin(); player != players.end(); ++player) {
        if (SqlDBWriter::savePlayer(player.value()) < 0) {
            QuasarAppUtils::Params::verboseLog("writeUpdatePlayerIntoDB failed when"
                                               " work globalUpdateDataRelease!!! id=" +
                                                QString::number(player.key()),
                                               QuasarAppUtils::VerboseLvl::Error);

        }
    }

    for (auto owner = owners.begin(); owner != owners.end(); ++owner) {
        if (!SqlDBWriter::saveowners(owner.key(), owner.value())) {
            QuasarAppUtils::Params::verboseLog("UpdateInfoOfowners failed when"
                                               " work globalUpdateDataRelease!!! id=" +
                                                QString::number(owner.key()),
                                               QuasarAppUtils::VerboseLvl::Error);
        }
    }

    lastUpdateTime = currentTime;
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

bool SqlDBCache::itemIsFreeFrom(int item) const {
    return SqlDBWriter::itemIsFreeFrom(item);
}

SqlDBCache::SqlDBCache(qint64 updateInterval) {
    lastUpdateTime = QDateTime::currentMSecsSinceEpoch();
    this->updateInterval = updateInterval;
}

SqlDBCache::~SqlDBCache() {
    globalUpdateDataBase(SqlDBCasheWriteMode::Force);
}

bool SqlDBCache::initDb(const QString &pdbath) {
    if (!SqlDBWriter::initDb(pdbath)) {
        return false;
    }

    getItem(getLastIdItems());
    getPlayer(getLastIdPlayers());

    return true;
}

Item SqlDBCache::getItem(int id) {
    if (!isValid()) {
        return Item();
    }

    auto item = items.value(id);

    if (item.isValid()) {
        return item;
    }

    item = SqlDBWriter::getItem(id);
    if (item.isValid()) {
        items.insert(id, item);
        return item;
    }

    return Item();
}

int SqlDBCache::saveItem(const Item &saveData) {
    if (!isValid()) {
        return -1;
    }
    auto item = saveData;

    int id = item.getId();

    if (id < 0) {
        id = generateIdForItem();
        if (!item.setId(id)) {
            return -1;
        }
    }

    if (!item.isValid()) {
        return -1;
    }
    items.insert(id, item);

    globalUpdateDataBase(SqlDBCasheWriteMode::On_New_Thread);

    emit sigItemChanged(id, item);

    return id;
}

PlayerDBData SqlDBCache::getPlayer(int id) {
    if (!isValid()) {
        return PlayerDBData();
    }

    auto player = players.value(id);

    if (player.isValid()) {
        return player;
    }

    player = SqlDBWriter::getPlayer(id);
    if (player.isValid()) {
        players.insert(id, player);
        return player;
    }

    return PlayerDBData();
}

int SqlDBCache::savePlayer(const PlayerDBData &saveData) {
    if (!isValid()) {
        return -1;
    }

    auto player = saveData;

    int id = player.id();

    if (id < 0) {
        id = generateIdForPalyer();
        player.setId(id);
    }

    if (!player.isValid()) {
        return -1;
    }

    int curSnake = player.getCureentSnake();

    if (curSnake >= 0 && !checkItem(curSnake, id)) {
        return -1;
    }

    players.insert(id, player);
    playersIds.insert(player.getGmail(), id);

    globalUpdateDataBase(SqlDBCasheWriteMode::On_New_Thread);
    emit sigPlayerChanged(id, player);

    return id;
}

bool SqlDBCache::login(const QString &gmail, const QString &pass) {
    if (!isValid()) {
        return false;
    }

    int id = getPlayerId(gmail);
    if (id < 0) {
        return false;
    }

    auto player = getPlayer(id);

    if (!player.isValid()) {
        return false;
    }

    return player.getHexPass() == pass;
}

bool SqlDBCache::giveAwayItem(int player, int item) {
    if (!isValid()) {
        return false;
    }

    if (!checkItem(item, player)) {
        return false;
    }

    if (owners.contains(player)) {
        auto &owner = owners[player];
        owner.remove(item);

        auto &p = players[player];

        if (p.getCureentSnake() == item) {
            p.setCureentSnake(-1);
        }

        globalUpdateDataBase(SqlDBCasheWriteMode::On_New_Thread);

        return true;
    }

    return false;
}

bool SqlDBCache::getItem(int player, int item, bool check) {
    if (!isValid()) {
        return false;
    }

    if (!(checkPlayer(player) && checkItem(item))) {
        return false;
    }

    if (check && !itemIsFreeFrom(item)) {
        return false;
    }

    if (owners.contains(player)) {
        auto &owner = owners[player];
        owner.insert(item);

        globalUpdateDataBase(SqlDBCasheWriteMode::On_New_Thread);

        return true;
    }

    QSet<int> items;
    if (!getAllItemsOfPalyer(player, items)) {
        return false;
    }

    if (owners.contains(player)) {
        auto &owner = owners[player];
        owner.insert(item);

        globalUpdateDataBase(SqlDBCasheWriteMode::On_New_Thread);

        return true;
    }

    return false;
}

bool SqlDBCache::moveItem(int owner, int receiver, int item) {
    if (!(giveAwayItem(owner, item) && getItem(receiver, item, false))) {
        return false;
    }

    sigItemChanged(item, getItem(item));
    return true;
}

int SqlDBCache::getPlayerId(const QString &gmail) {
    int id = playersIds.value(gmail, -1);
    if (id < 0) {
        id = SqlDBWriter::getPlayerId(gmail);

        if (id >= 0) {
            playersIds[gmail] = id;
        }
        return id;
    }

    return id;
}

bool SqlDBCache::getAllItemsOfPalyer(int player, QSet<int> &items) {
    if (owners.contains(player)) {
        items = owners[player];
        return true;
    }

    if (SqlDBWriter::getAllItemsOfPalyer(player, items) &&
            checkPlayer(player)) {

        owners.insert(player, items);
        return true;
    }

    return false;
}
