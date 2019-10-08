#ifndef SQLDBWRITER_H
#define SQLDBWRITER_H
#include "item.h"

#include <QObject>
#include <QSqlDatabase>
#include <QDir>
#include <player.h>

#define DEFAULT_DB_NAME "SnakeDatabase.db"
#define DEFAULT_DB_PATH QDir::homePath() + "/SnakeServer/" + DEFAULT_DB_NAME
#define DEFAULT_UPDATE_INTERVAL 3600000 // 1 hour

class QSqlQuery;
class QSqlDatabase;
class QSqlQuery;
class PlayerDBData;

class SERVERSHARED_EXPORT SqlDBWriter
{
private:
    bool exec(QSqlQuery *sq, const QString &sqlFile);

    QSqlDatabase *db = nullptr;
    QSqlQuery *query = nullptr;
    bool initSuccessful = false;

    bool enableFK() const;
    bool disableFK() const;

protected:
    int getLastIdItems();
    int getLastIdPlayers();

    virtual int getPlayerId(const QString &gmail);

    virtual bool checkPlayer(int id);
    virtual bool checkItem(int idItem, int idOwner = -1);

    virtual int savePlayer(const PlayerDBData& player);
    virtual int saveItem(const Item &item);
    virtual bool saveowners(int player, const QSet<int>);

    virtual bool getAllItemsOfPalyer(int player, QSet<int>& items);

    virtual PlayerDBData getPlayer(int id);
    virtual Item getItem(int id);

    virtual bool itemIsFreeFrom(int item) const;

public:
    SqlDBWriter();

    virtual bool initDb(const QString &path = DEFAULT_DB_PATH);

    virtual bool isValid() const;

    virtual ~SqlDBWriter();

    friend class testSankeServer;

};

#endif // SQLDBWRITER_H
