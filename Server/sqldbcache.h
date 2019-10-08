#ifndef SQLDBCASHE_H
#define SQLDBCASHE_H
#include "sqldbwriter.h"

#include <QMap>
#include <QHash>
#include <QSet>
#include <QVariantMap>
#include <player.h>
#include "item.h"
#include <clientprotocol.h>

enum class SqlDBCasheWriteMode: int {
    Default = 0x0,
    On_New_Thread = 0x1,
    Force = 0x2,
} ;

namespace ClientProtocol {
    class BaseNetworkObject;
}

class SERVERSHARED_EXPORT SqlDBCache: public QObject , private SqlDBWriter
{
    Q_OBJECT
private:
    qint64 lastUpdateTime = 0;
    qint64 updateInterval = DEFAULT_UPDATE_INTERVAL;

    QMap <int, Item> items;
    QMap <int, PlayerDBData> players;
    QHash <int, QSet<int>>  owners;
    QHash <QString, int>  playersIds;

    int generateIdForItem() const;
    int generateIdForPalyer() const;

    bool checkPlayer(int id) override;
    bool checkItem(int idItem, int idOwner = -1) override;

    void globalUpdateDataBasePrivate(qint64 currentTime);
    void globalUpdateDataBase(SqlDBCasheWriteMode mode = SqlDBCasheWriteMode::Default);

    bool itemIsFreeFrom(int item) const override ;

public:
    SqlDBCache(qint64 updateInterval = DEFAULT_UPDATE_INTERVAL);
    ~SqlDBCache() override;

    bool initDb(const QString &pdbath = DEFAULT_DB_PATH) override;

    Item getItem(int id) override;
    int saveItem(const Item& saveData) override;
    PlayerDBData getPlayer(int id) override;
    int savePlayer(const PlayerDBData &player) override;

    bool login(const QString& gmail, const QString& pass);

    bool giveAwayItem(int player, int item);
    bool getItem(int player, int item, bool check = true);
    bool moveItem(int owner, int receiver, int item);
    int getPlayerId(const QString &gmail) override;

    bool getAllItemsOfPalyer(int player, QSet<int>& items) override;

    friend class testSankeServer;

signals:
    void sigItemChanged(int id, const Item& newData);
    void sigPlayerChanged(int id, const PlayerDBData& newData);

};

#endif // SQLDBCASHE_H
