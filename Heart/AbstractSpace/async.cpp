#include "async.h"

#include <QCoreApplication>
#include <QDateTime>

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

void Async::asyncLauncher(std::function<bool ()> job,
                          bool *resultOfWork,
                          bool *endOfWork) const {

    bool result = job();

    if (resultOfWork)
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

bool Async::waitFor(const std::function<bool ()> &condition, int timeout) const {
    waitPrivate(condition(), timeout)
}

}
