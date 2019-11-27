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
    bool saveObject(const QWeakPointer<DBObject> &saveObject) override;
    bool deleteObject(const QWeakPointer<DBObject> & deleteObject) override;

private slots:
    void handleSaveObject(QSharedPointer<DBObject> saveObject);
    void handleDeleteObject(QSharedPointer<DBObject> deleteObject);

};

}
#endif // ASYNCSQLDBWRITER_H
