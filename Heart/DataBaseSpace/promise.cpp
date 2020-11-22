#include "promise.h"

#include <QCoreApplication>
#include <QDateTime>

bool QH::waitFor(const std::function<bool()> &condition, int timeout) {
    auto curmsec = QDateTime::currentMSecsSinceEpoch() + timeout;
    while (curmsec > QDateTime::currentMSecsSinceEpoch() && !condition()) {
        QCoreApplication::processEvents();
    }
    QCoreApplication::processEvents();
    return condition();
}
