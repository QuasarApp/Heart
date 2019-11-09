#ifndef ASYNCSQLDBWRITER_H
#define ASYNCSQLDBWRITER_H

#include "sqldbwriter.h"

namespace NetworkProtocol {


/**
 * @brief The AsyncSqlDbWriter class some as SqlDBWriter bud run all commnad in own thread
 */
class AsyncSqlDbWriter :public QObject, public SqlDBWriter
{
    Q_OBJECT
public:
    AsyncSqlDbWriter(QObject* ptr = nullptr);

    // iObjectProvider interface
    bool saveObject(QWeakPointer<DBObject> saveObject) override;
    bool deleteObject(const QString &table, int id) override;

private slots:
    void handleSaveObject(QWeakPointer<DBObject> saveObject);
    void handleDeleteObject(const QString &table, int id);

};

}
#endif // ASYNCSQLDBWRITER_H
