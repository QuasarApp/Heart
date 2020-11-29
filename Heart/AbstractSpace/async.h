#ifndef ASYNC_H
#define ASYNC_H
#include <QObject>
#include <functional>
#include "config.h"

namespace QH {

/**
 * @brief The Async class This is bandele of async templates and async wrapers.
 * @note If you use this object then you do not need include QObject becouse this class include the  QObject class.
 * Do not forgae the Q_OBJECT mactoss.
 */
class Async: public QObject {

    Q_OBJECT

protected:
    Async(QObject* ptr = nullptr);

    /**
     * @brief asyncLauncher This is base async launcher method for move jobs to new thread.
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
                                            "asyncLauncher",
                                            Qt::QueuedConnection,
                                            Q_ARG(std::function<bool()>, job),
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
                                            "asyncLauncher",
                                            Qt::QueuedConnection,
                                            Q_ARG(std::function<bool()>, job));

            return invoke;
     * \endcode
     */
    void asyncLauncher(std::function<bool()> job,
                       bool* resultOfWork = nullptr,
                       bool* endOfWork = nullptr) const;

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
    bool waitFor(const std::function<bool()> &condition, int timeout = WAIT_TIME) const;
};

}

Q_DECLARE_METATYPE(std::function<bool()>)
#endif // ASYNC_H
