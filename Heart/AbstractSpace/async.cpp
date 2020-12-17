#include "async.h"

#include <QCoreApplication>
#include <QDateTime>
#include <QThread>
#include <QDebug>

// Private implementation of waitFor functions.
#define waitPrivate(CONDITION, TIMEOUT) \
    auto curmsec = QDateTime::currentMSecsSinceEpoch() + TIMEOUT; \
    while (curmsec > QDateTime::currentMSecsSinceEpoch() && !CONDITION) { \
        QCoreApplication::processEvents(); \
    } \
    QCoreApplication::processEvents(); \
    return CONDITION; \


namespace QH {
Async::Async(QObject *ptr):
    QObject(ptr) {

}

void Async::asyncHandler(Job job,
                          bool *endOfWork,
                          bool *resultOfWork) const {

    bool result = job();

    if (endOfWork && resultOfWork)
        *resultOfWork = result;

    if (endOfWork) {
        *endOfWork = true;
    }


}

bool Async::waitFor(bool *condition, int timeout) const {
    if (!condition)
        return false;

    waitPrivate(*condition, timeout)
}

bool Async::asyncLauncher(const Async::Job &job, bool await) {
    if (QThread::currentThread() == thread()) {
        return job();
    }

    if (!await) {
        return  QMetaObject::invokeMethod(this,
                                       "asyncHandler",
                                       Qt::QueuedConnection,
                                       Q_ARG(QH::Async::Job, job));
    }

    bool workOfEnd = false, workResult = false;

    bool invockeResult = QMetaObject::invokeMethod(this,
                                                   "asyncHandler",
                                                   Qt::QueuedConnection,
                                                   Q_ARG(QH::Async::Job, job),
                                                   Q_ARG(bool *, &workOfEnd),
                                                   Q_ARG(bool *, &workResult));


    if (!invockeResult)
        return false;

    if (!waitFor(&workOfEnd)) {
        return false;
    }

    return workResult;
}

bool Async::waitFor(const std::function<bool ()> &condition, int timeout) const {
    waitPrivate(condition(), timeout)
}

}
