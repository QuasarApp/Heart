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

namespace QH {


/**
 * @brief The AsyncSqlDbWriter class is some as SqlDBWriter bud run all commnad in own thread
 * This class is thread save.
 */
class AsyncSqlDbWriter :public QObject, public SqlDBWriter
{
    Q_OBJECT
public:
    AsyncSqlDbWriter(QObject* ptr = nullptr);
    ~AsyncSqlDbWriter();

    bool updateObject(const PKG::DBObject* saveObject) override;
    bool deleteObject(const PKG::DBObject* deleteObject) override;
    bool insertObject(const PKG::DBObject *updateObject) override;

    /**
     * @brief updateObjectWithWait this method is wraper of the updateObject method with waiting of works result.
     * @note This method stop current thread while the request is not finished.
     * @param updateObject This is  pointer to the update object.
     * @return true if function finished successful.
     */
    bool updateObjectWithWait(const PKG::DBObject* updateObject);

    /**
     * @brief insertObjectWithWait this method is wraper of the insertObject method with waiting of works result.
     * @note This method stop current thread while the request is not finished.
     * @param inserObject This is  pointer to the insert object.
     * @return true if function finished successful.
     */
    bool insertObjectWithWait(const PKG::DBObject* inserObject);

    /**
     * @brief deleteObjectWithWait this method is wraper of the deleteObject method with waiting of works result.
     * @note This method stop current thread while the request is not finished.
     * @param deleteObject This is  pointer to the saved object.
     * @return true if function finished successful
     */
    bool deleteObjectWithWait(const PKG::DBObject* deleteObject);

    /**
     * @brief getAllObjects this implementation work on own thread and wait results in current thread.
     */
    bool getAllObjects(const PKG::DBObject &templateObject, QList<const PKG::DBObject *> &result) override;
    bool initDb(const QVariantMap &params) override;

protected slots:
    /**
     * @brief handleUpdateObject this method call the SqlDBWriter::updateObject method on an own thread.
     * @param saveObject This is need to updated object.
     * @param resultOfWork This is bool variable contais result of work a SqlDBWriter::saveObject method.
     * @param endOfWork This wariable set true when the SqlDBWriter::updateObject is finished.
     */
    void handleUpdateObject(const QH::PKG::DBObject* saveObject,
                           bool *resultOfWork, bool *endOfWork);

    /**
     * @brief handleDeleteObject this method call the SqlDBWriter::deleteObject method on an own thread.
     * @param deleteObject is deleted object.
     * @param resultOfWork This is bool variable contais result of work a SqlDBWriter::saveObject method.
     * @param endOfWork This wariable set true when the SqlDBWriter::saveObject is finished.
     */
    void handleDeleteObject(const QH::PKG::DBObject* deleteObject,
                            bool *resultOfWork, bool *endOfWork);

    /**
     * @brief handleInsertObject this method call the SqlDBWriter::insertObject method on an own thread.
     * @param object This is need to update object.
     * @param resultOfWork This is bool variable contais result of work a SqlDBWriter::saveObject method.
     * @param endOfWork This wariable set true when the SqlDBWriter::saveObject is finished.
     */
    void handleInsertObject(const QH::PKG::DBObject *object,
                            bool *resultOfWork,
                            bool *endOfWork);

    /**
     * @brief handleGetAllObject This method call getAllObjects on own thread.
     * @note This method stop current thread while the request is not finished.
     * @param templateObject - the some as in SqlDBWriter::getAllObjects
     * @param result this is some as in getAllObjects
     * @param resultOfWork This is bool variable contais result of work a SqlDBWriter::saveObject method.
     * @param endOfWork This wariable set true when the SqlDBWriter::saveObject is finished.
     */
    virtual void handleGetAllObject(const QH::PKG::DBObject *templateObject,
                                    QList<const QH::PKG::DBObject *> *result,
                                    bool *resultOfWork, bool *endOfWork = nullptr);

    /**
     * @brief handleInitDb This method invoke initDb on own thread
     * @note This method stop current thread while the request is not finished.
     * @param params This is input parameters data. for more information see the SqlDBWriter::defaultInitPararm method.
     * @param resultOfWork This is bool variable contais result of work a SqlDBWriter::saveObject method.
     * @param endOfWork This wariable set true when the SqlDBWriter::saveObject is finished.
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
