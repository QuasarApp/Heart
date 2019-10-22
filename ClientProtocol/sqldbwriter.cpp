#include "playerdbdata.h"
#include "sqldbwriter.h"

#include <QRegularExpression>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QDebug>
#include <QSqlError>
#include <quasarapp.h>
#include <clientprotocol.h>
#include <factorynetobjects.h>

bool SqlDBWriter::exec(QSqlQuery *sq,const QString& sqlFile) {
    QFile f(sqlFile);
    bool result = true;
    if (f.open(QIODevice::ReadOnly)) {

        QString temp, delimiter = ";";
        QTextStream stream(&f);
        stream.setCodec("UTF8");

        while(!stream.atEnd()) {
            temp += stream.readLine();
            if (temp.lastIndexOf("delimiter", -1, Qt::CaseInsensitive) > -1) {
                temp.remove("delimiter", Qt::CaseInsensitive);
                int last = temp.indexOf(QRegularExpression("[^ \f\n\r\t\v]")) + 1;
                int begin = temp.lastIndexOf(QRegularExpression("[^ \f\n\r\t\v]"));
                delimiter = temp.mid(begin, last - begin);
                temp = "";
            } else {
                if (temp.lastIndexOf(delimiter) >- 1) {
                    temp.remove(delimiter);
                    result = result && sq->exec(temp);

                    if (!result) {
                        qCritical() << sq->lastError().text();
                        f.close();
                        return false;
                    }

                    temp = "";
                }
            }
        }

        f.close();
        return result;
    }
    return false;
}

bool SqlDBWriter::enableFK() const {

    QString request = QString("PRAGMA foreign_keys = ON");
    if (!query->exec(request)) {
        QuasarAppUtils::Params::verboseLog("request error : " + query->lastError().text());
        return false;
    }

    return true;
}

bool SqlDBWriter::disableFK() const {


    QString request = QString("PRAGMA foreign_keys = OFF");
    if (!query->exec(request)) {
        QuasarAppUtils::Params::verboseLog("request error : " + query->lastError().text());
        return false;
    }

    return true;

}

int SqlDBWriter::getLastIdItems() {
    if (!SqlDBWriter::isValid()) {
        return -1;
    }

    QString request = QString("SELECT MAX(id) FROM items");
    if (!query->exec(request)) {
        QuasarAppUtils::Params::verboseLog("request error : " + query->lastError().text());
        return -1;
    }

    if (!query->next()) {
        return -1;
    }

    auto res = query->value(0).toInt();
    return res;
}

int SqlDBWriter::getLastIdPlayers() {
    if (!SqlDBWriter::isValid()) {
        return -1;
    }

    QString request = QString("SELECT MAX(id) FROM players");
    if (!query->exec(request)) {
        QuasarAppUtils::Params::verboseLog("request error : " + query->lastError().text());
        return -1;
    }

    if (!query->next()) {
        return -1;
    }

    return query->value(0).toInt();
}

int SqlDBWriter::getPlayerId(const QString &gmail) {

    if (!SqlDBWriter::isValid()) {
        return -1;
    }

    QString request = QString("SELECT id from players where gmail='%0'").arg(gmail);

    if (!query->exec(request)) {
        QuasarAppUtils::Params::verboseLog("request error : " + query->lastError().text());
        return -1;
    }

    if (!query->next()) {
        return -1;
    }

    return query->value("id").toInt();
}

bool SqlDBWriter::checkPlayer(int id) {

    QString request = QString("SELECT id from players where id='%0'").arg(id);

    if (!query->exec(request)) {
        QuasarAppUtils::Params::verboseLog("request error : " + query->lastError().text());
        return false;
    }

    if (!query->next()) {
        return false;
    }

    return true;
}

bool SqlDBWriter::checkItem(int idItem, int idOwner) {


    if (idOwner >= 0 ) {
        if (!SqlDBWriter::checkPlayer(idOwner)) {
            return false;
        }

        QString request = QString("SELECT item from owners where player='%0' and item='%1'").
                arg(idOwner).arg(idItem);

        if (!query->exec(request)) {
            QuasarAppUtils::Params::verboseLog("request error : " + query->lastError().text());
            return false;
        }

        if (!query->next()) {
            return false;
        }

        return true;
    }

    QString request = QString("SELECT id from items where id='%0'").
            arg(idItem);

    if (!query->exec(request)) {
        QuasarAppUtils::Params::verboseLog("request error : " + query->lastError().text());
        return false;
    }

    if (!query->next()) {
        return false;
    }

    return true;
}

int SqlDBWriter::savePlayer(const PlayerDBData &player) {
    if (!SqlDBWriter::isValid()) {
        return -1;
    }

    if (!player.isValid()) {
        return -1;
    }

    QString request;
    int id = player.id();

    int curSnake = player.getCureentSnake();

    if (curSnake >= 0 && !SqlDBWriter::checkItem(curSnake, id)) {
        return -1;
    }


    if (SqlDBWriter::checkPlayer(id)) {
        request = QString("UPDATE players SET name='%0', gmail='%1', pass='%2', money='%3',"
                          " avgrecord='%4', record='%5', lastOnline='%6',"
                          " onlinetime='%7', currentsnake='%8' WHERE id='%9' ").arg(
                    player.getName()).arg(
                    player.getGmail()).arg(
                    player.getHexPass()).arg(
                    player.getMany()).arg(
                    player.getAvgRecord()).arg(
                    player.getRecord()).arg(
                    player.getLastOnline()).arg(
                    player.getOnlineTime()).arg(
                    (curSnake >= 0)? QString::number(curSnake) : "NULL").arg(
                    id);

    } else {
        request = QString("INSERT INTO players(id, name, gmail, pass, money, avgrecord, record,"
                          " lastOnline, onlinetime, currentsnake) VALUES "
                          "('%0', '%1', '%2', '%3', '%4', '%5', '%6', '%7', '%8', '%9')").arg(
                    id).arg(
                    player.getName()).arg(
                    player.getGmail()).arg(
                    player.getHexPass()).arg(
                    player.getMany()).arg(
                    player.getAvgRecord()).arg(
                    player.getRecord()).arg(
                    player.getLastOnline()).arg(
                    player.getOnlineTime()).arg(
                    (curSnake >= 0)? QString::number(curSnake) : "NULL");

    }


    if (curSnake < 0 && !disableFK()) {
        return false;
    }


    if (!query->exec(request)) {
        QuasarAppUtils::Params::verboseLog("request error : " + query->lastError().text());
        return -1;
    }

    if (curSnake < 0 && !enableFK()) {
        return -1;
    }

    return id;
}

int SqlDBWriter::saveItem(const Item &item) {
    if (!SqlDBWriter::isValid()) {
        return -1;
    }

    if (!item.isValid()) {
        return -1;
    }

    auto type = item.cmd();
    int id = item.getId();

    QByteArray bytes = item.dataArray();
    QString request;

    if (SqlDBWriter::checkItem(id)) {
        request = QString("UPDATE items SET type='%1', data = :bytes where id = %0").
                arg(id).
                arg(static_cast<int>(type));
    } else {
        request = QString("INSERT INTO items(id, type, data) VALUES"
                          "('%0', '%1', :bytes)").
                arg(id).
                arg(static_cast<int>(type));
    }

    if (!query->prepare(request)) {
        QuasarAppUtils::Params::verboseLog("request error : " + query->lastError().text());
        return -1;
    }

    query->bindValue(":bytes", bytes);

    if (!query->exec()) {
        QuasarAppUtils::Params::verboseLog("request error : " + query->lastError().text());
        return -1;
    }

    return id;
}

bool SqlDBWriter::getAllItemsOfPalyer(int player, QSet<int> &items) {
    if (!SqlDBWriter::isValid()) {
        return false;
    }

    QString request = QString("SELECT item from owners where player='%0'").arg(player);

    if (!query->exec(request)) {
        QuasarAppUtils::Params::verboseLog("request error : " + query->lastError().text());
        return false;
    }

    while (query->next()) {
        items.insert(query->value(0).toInt());
    }

    return true;
}

bool SqlDBWriter::saveowners(int player, const QSet<int> items) {

    if (!SqlDBWriter::isValid()) {
        return false;
    }

    QString request = QString("DELETE from owners where player='%0' ").
            arg(player);

    if (!query->exec(request)) {
        QuasarAppUtils::Params::verboseLog("request error : " + query->lastError().text());
        return false;
    }

    if (items.isEmpty()) {
        return true;
    }

    request = QString("INSERT INTO owners(player, item) VALUES ");
    for (int item: items) {
        request.push_back("(" + QString::number(player) + "," + QString::number(item) + ")");
    }

    if (!query->exec(request)) {
        QuasarAppUtils::Params::verboseLog(request);
        QuasarAppUtils::Params::verboseLog("request error : " + query->lastError().text());
        return false;
    }

    return true;
}

PlayerDBData SqlDBWriter::getPlayer(int id) {

    if (!SqlDBWriter::isValid()) {
        return PlayerDBData();
    }

    QString request = QString("SELECT * FROM players WHERE id=%0").arg(id);
    if (!query->exec(request)) {
        QuasarAppUtils::Params::verboseLog("request error : " + query->lastError().text());
        return PlayerDBData();
    }

    if (!query->next()) {
        return PlayerDBData();
    }

    auto player = PlayerDBData();

    player.setId(id);
    player.setName(query->value("name").toString());
    player.setGmail(query->value("gmail").toString());
    player.fromHexPass(query->value("pass").toString());
    player.setMany(query->value("money").toUInt());
    player.setAvgRecord(query->value("avgrecord").toUInt());
    player.setRecord(query->value("record").toUInt());
    player.setLastOnline(query->value("lastOnline").toInt());
    player.setOnlineTime(query->value("onlinetime").toInt());
    player.setCureentSnake(query->value("currentsnake").toInt());

    return player;
}

Item SqlDBWriter::getItem(int id) {

    if (!SqlDBWriter::isValid()) {
        return Item();
    }

    QString request = QString("SELECT type, data FROM items WHERE id=%0").arg(id);
    if (!query->exec(request)) {
        QuasarAppUtils::Params::verboseLog("request error : " + query->lastError().text());
        return Item();
    }

    if (!query->next()) {
        return Item();
    }

    auto type = static_cast<ClientProtocol::Command>(query->value(0).toUInt());
    auto data = query->value(1).toByteArray();

    return Item(type, data);
}

bool SqlDBWriter::itemIsFreeFrom(int item) const {
    if (!SqlDBWriter::isValid()) {
        return false;
    }

    QString request = QString("SELECT player FROM owners WHERE player=%0").arg(item);
    if (!query->exec(request)) {
        QuasarAppUtils::Params::verboseLog("request error : " + query->lastError().text());
        return false;
    }

    return !query->next();
}

SqlDBWriter::SqlDBWriter() {

}

bool SqlDBWriter::initDb(const QString &databasePath) {
    QStringList drivers = QSqlDatabase::drivers();
    db = new QSqlDatabase();

    *db = QSqlDatabase::addDatabase("QSQLITE", QFileInfo(databasePath).fileName());
    db->setDatabaseName(QFileInfo(databasePath).absoluteFilePath());
    query = new QSqlQuery(*db);

    if (!QDir("").mkpath(QFileInfo(databasePath).absolutePath())) {
        return false;
    }

    if (!db->open()) {
        return false;
    }

    if (!exec(query, ":/sql/DB")) {
        return false;
    }

    initSuccessful = true;
    return initSuccessful;
}

bool SqlDBWriter::isValid() const {
    if (!db) {
        return false;
    }

    return db->isValid() && db->isOpen() && initSuccessful;
}

SqlDBWriter::~SqlDBWriter() {
    if (db) {
        delete db;
    }

    if (query) {
        delete query;
    }
}
