#ifndef ASYNCSQLDBWRITER_H
#define ASYNCSQLDBWRITER_H

#include "sqldbwriter.h"

namespace NP {


/**
 * @brief The AsyncSqlDbWriter class some as SqlDBWriter bud run all commnad in own thread
 */
class AsyncSqlDbWriter :public QObject, public SqlDBWriter
{
    Q_OBJECT
public:
    AsyncSqlDbWriter(QObject* ptr = nullptr);

    // iObjectProvider interface
    bool saveObject(const WP<AbstractData> &saveObject) override;
    bool deleteObject(const WP<AbstractData> & deleteObject) override;

private slots:
    void handleSaveObject(SP<DBObject> saveObject);
    void handleDeleteObject(SP<DBObject> deleteObject);

};

}
#endif // ASYNCSQLDBWRITER_H
