/*
 * Copyright (C) 2018-2020 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
*/

#ifndef ASYNCSQLDBWRITER_H
#define ASYNCSQLDBWRITER_H

#include "sqldbwriter.h"
#include "atomicmetatypes.h"

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
    ~AsyncSqlDbWriter();
    // iObjectProvider interface
    /**
     * @brief saveObject - save object in to database. This implementation work on own thread
     * @param saveObject - ptr to object
     * @return true if function finished successful
     */
    bool saveObject(const DBObject* saveObject) override;

    /**
     * @brief saveObject - delete object in to database. This implementation work on own thread
     * @param deleteObject - ptr to object
     * @return true if function finished successful
     */
    bool deleteObject(const DBObject* deleteObject) override;

    /**
     * @brief saveObjectWithWait - this is owerload of saveObject with wait results of a database thread.
     * @param saveObject - ptr to object
     * @return true if function finished successful
     */
    bool saveObjectWithWait(const DBObject* saveObject);

    /**
     * @brief deleteObjectWithWait - this is owerload of deleteObject with wait results of a database thread.
     * @param deleteObject - ptr to object
     * @return true if function finished successful
     */
    bool deleteObjectWithWait(const DBObject* deleteObject);

    /**
     * @brief getAllObjects - this implementation work on own thread and wait results in current thread.
     * @param templateObject - template object with request
     * @param result - list of objects
     * @return true if function finished successful
     */
    bool getAllObjects(const DBObject &templateObject, QList<const DBObject *> &result) override;

    /**
     * @brief initDb - this implementation initialise database in own thread and wait result of initialization on current thread
     * @param initDbParams - initialise parameters
     * @return true if database initialise successful
     */
    bool initDb(const QVariantMap &params) override;

protected slots:
    /**
     * @brief handleSaveObject - this method call SaveObject on own thread.
     * @param saveObject - object for save
     */
    void handleSaveObject(const NP::DBObject* saveObject,
                           bool *resultOfWork, bool *endOfWork);

    /**
     * @brief handleDeleteObject - this method call DeleteObject on own thread.
     * @param deleteObject object for delete
     */
    void handleDeleteObject(const NP::DBObject* deleteObject,
                            bool *resultOfWork, bool *endOfWork);

    /**
     * @brief the handleGetAllObject - this method call getAllObjects on own thread.
     * @param templateObject - the some as in getAllObjects
     * @param result - the some as in getAllObjects
     * @param resultOfWork - this ptr contains result of invoked of getAllObjects method on own thread
     * @param endOfWork - this ptr set true when invocked method is finished
     * @param cb - this call back method invoke after getAllObjects method
     */
    virtual void handleGetAllObject(const NP::DBObject *templateObject, QList<const DBObject *> *result,
                                    bool *resultOfWork, bool *endOfWork = nullptr);

    /**
     * @brief handleInitDb - this method invoke initDb on own thread
     * @param params - input parameters data
     * @param resultOfWork - this ptr contains result of invoked of initDb method on own thread
     * @param endOfWork - this ptr set true when invocked method is finished
     */
    void handleInitDb(const QVariantMap &params,
                      bool *resultOfWork, bool *endOfWork = nullptr);

private:
    /**
     * @brief waitFor - The base wait function.
     * @param condition - condition for wait
     * @param timeout - maximu time for wait. By default this value equals WAIT_TIME it is 30000 msec.
     * @return true if condition is true.
     */
    bool waitFor(bool* condition, int timeout = WAIT_TIME) const;

    QThread *_own = nullptr;

};

}
#endif // ASYNCSQLDBWRITER_H
