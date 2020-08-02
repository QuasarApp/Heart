/*
 * Copyright (C) 2018-2020 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
*/

#ifndef ASYNCSQLDBWRITER_H
#define ASYNCSQLDBWRITER_H

#include "sqldbwriter.h"

namespace NP {


/**
 * @brief The AsyncSqlDbWriter class some as SqlDBWriter bud run all commnad in main thread
 * is thread save db writer.
 */
class AsyncSqlDbWriter :public QObject, public SqlDBWriter
{
    Q_OBJECT
public:
    AsyncSqlDbWriter(QObject* ptr = nullptr);

    // iObjectProvider interface
    bool saveObject(const DBObject* saveObject) override;
    bool deleteObject(const DBObject* deleteObject) override;
    bool getAllObjects(const DBObject &templateObject, QList<DBObject *> &result) override;

protected slots:
    void handleSaveObject(const DBObject* saveObject);
    void handleDeleteObject(const DBObject* deleteObject);

    /**
     * @brief the handleGetAllObject - this method call getAllObjects on main thread.
     * @arg templateObject - the some as in getAllObjects
     * @arg result - the some as in getAllObjects
     * @arg resultOfWork - this ptr contains result of invoked of getAllObjects method on main thread
     * @arg endOfWork - this ptr set true when invocked method is finished
     * @arg cb - this call back method invoke after getAllObjects method
     */
    virtual void handleGetAllObject(const DBObject &templateObject, QList<DBObject *> &result,
                                    bool *resultOfWork, bool *endOfWork = nullptr);

private:
    bool waitFor(bool* condition, int timeout = 30000) const;

    Qt::HANDLE _mainThreadId;

};

}
#endif // ASYNCSQLDBWRITER_H
