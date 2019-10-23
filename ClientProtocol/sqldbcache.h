#ifndef SQLDBCASHE_H
#define SQLDBCASHE_H

#include <QMap>
#include <QHash>
#include <QSet>
#include <QVariantMap>
#include <clientprotocol.h>

namespace ClientProtocol {

class SqlDBWriter;
class DBObject;

enum class SqlDBCasheWriteMode: int {
    Default = 0x0,
    On_New_Thread = 0x1,
    Force = 0x2,
} ;

class CLIENTPROTOCOLSHARED_EXPORT SqlDBCache: public QObject
{
    Q_OBJECT


public:
    SqlDBCache(qint64 updateInterval = DEFAULT_UPDATE_INTERVAL);
    ~SqlDBCache() override;

    SqlDBWriter *writer() const;
    void setWriter(SqlDBWriter *writer);

protected:
    virtual bool init();

private:
    qint64 lastUpdateTime = 0;
    qint64 updateInterval = DEFAULT_UPDATE_INTERVAL;

    SqlDBWriter *_writer = nullptr;

    QHash<QString, QHash <int, DBObject*>>  _cache;

    int generateIdForItem() const;
    int generateIdForPalyer() const;

    void globalUpdateDataBasePrivate(qint64 currentTime);
    void globalUpdateDataBase(SqlDBCasheWriteMode mode = SqlDBCasheWriteMode::Default);

signals:
    void sigItemChanged(int id);

};

}
#endif // SQLDBCASHE_H
