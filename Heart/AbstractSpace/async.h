/*
 * Copyright (C) 2018-2021 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
*/

#ifndef ASYNC_H
#define ASYNC_H
#include <QObject>
#include <functional>
#include "config.h"
#include "atomicmetatypes.h"

namespace QH {

/**
 * @brief The Async class This is bandele of async templates and async wrapers.
 * @note If you use this object then you do not need include QObject becouse this class include the  QObject class.
 * Do not forgae the Q_OBJECT mactoss.
 */
class Async: public QObject {

    Q_OBJECT

public:
    /**
     * The Job is wrapper of the std::function<bool()> type. This type registered like the Qt meta type
     * for using in the qt metasystem.
     */
    using Job = std::function<bool()>;


protected:
    Async(QObject* ptr = nullptr);

    /**
     * @brief waitFor This is base wait function.
     * @param condition This is pointer to awaiting boolean variable.
     * @param timeout This is maximu time for wait. By default this value equals WAIT_TIME it is 30000 msec.
     * @return true if condition is true.
     */
    bool waitFor(bool* condition, int timeout = WAIT_TIME) const;

    /**
     * @brief waitFor This is base wait function.
     * @param condition This is lambda method with condition results.
     * @param timeout This is maximu time for wait. By default this value equals WAIT_TIME it is 30000 msec.
     * @return true if condition is true.
     */
    bool waitFor(const Job &condition, int timeout = WAIT_TIME) const;

    /**
     * @brief asyncLauncher This method invoke a job on the thread (usnign the asyncHandler method) of this object.
     * @param job This is function with needed job.
     * @param await This is boolean option for enable or disable waiot of finish of the job function.
     * @return true if the job function started correctly. IF the await option is true then
     * this method return result of job function.
     */
    bool asyncLauncher(const Job &job, bool await = false);
private slots:

    /**
     * @brief asyncHandler async This is base async launcher method for move jobs to new thread.
     * @param job This is job to do.
     * @param resultOfWork This is pointer of bool value of a result of the job method.
     * @note If you want to disable check results just use nullptr value.
     * @param endOfWork This is pointer to bool value. If balue of this method changed to true then the hob method has ben finished.
     * For check result of the job dunction use the resultOfWork option.
     *
     * Example of use:
     *
     * Async Await:
     * \code{cpp}
     *
     *      auto job = [](){
     *          ...
     *          return true;
     *      }
     *      bool invoke = QMetaObject::invokeMethod(this,
                                            "asyncHandler",
                                            Qt::QueuedConnection,
                                            Q_ARG(QH::Async::Job, job),
                                            Q_ARG(bool *, &workResult),
                                            Q_ARG(bool *, &workOfEnd));

            if (!invoke) {
                return false;
            }


            if (!waitFor(&workOfEnd)) {
                return false;
            }

            return workResult;
     * \endcode
     *
     *
     * Async:
     * \code{cpp}
     *
     *      auto job = [](){
     *          ...
     *          return true;
     *      }
     *      bool invoke = QMetaObject::invokeMethod(this,
                                            "asyncHandler",
                                            Qt::QueuedConnection,
                                            Q_ARG(std::function<bool()>, job));

            return invoke;
     * \endcode
     *
     * @warning if you use the resultOfWork parameter then necessarily set the endOfWork parameter and wait the endOfWork condition. If you do not set this then this method broken the stack of the main thread.
     *
     */
    void asyncHandler (QH::Async::Job job,
                       bool* endOfWork = nullptr,
                       bool* resultOfWork = nullptr) const;


  };

}

Q_DECLARE_METATYPE(QH::Async::Job)
#endif // ASYNC_H
