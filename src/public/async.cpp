/*
 * Copyright (C) 2018-2025 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
*/

#include "abstractnode.h"
#include "async.h"

#include <QCoreApplication>
#include <QDateTime>
#include <QThread>
#include <QDebug>
#include <quasarapp.h>
#include <thread>
#include <chrono>
#include <qaglobalutils.h>

// Private implementation of waitFor functions.
#define waitPrivate(CONDITION, TIMEOUT) \
    auto curmsec = QDateTime::currentMSecsSinceEpoch() + TIMEOUT; \
    while (curmsec > QDateTime::currentMSecsSinceEpoch() && !CONDITION) { \
        QCoreApplication::processEvents(); \
    } \
    QCoreApplication::processEvents(); \
    return CONDITION; \


#define freazePrivate(CONDITION, TIMEOUT) \
    auto curmsec = QDateTime::currentMSecsSinceEpoch() + TIMEOUT; \
    while (curmsec > QDateTime::currentMSecsSinceEpoch() && !CONDITION) { \
        std::this_thread::sleep_for(std::chrono::microseconds(1)); \
    } \
    return CONDITION; \

namespace QH {
Async::Async(QThread *thread, QObject *ptr):
    QObject(ptr) {

    threadAnalize(thread);
    moveToThread(thread);
}

Async::~Async() {

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

void Async::threadAnalize(QThread *thread) {
    QThread * mainThread = AbstractNode::mainThreadID();

    debug_assert((mainThread != thread) && thread, "You try create async object into main thread");
}

bool Async::waitFor(bool *condition, int timeout, bool freaze) const {
    if (!condition)
        return false;

    if (freaze) {
        freazePrivate(*condition, timeout)
    } else {
        waitPrivate(*condition, timeout)
    }
}

bool Async::asyncLauncher(const Async::Job &job, bool await, bool freaze) const {

    if (QThread::currentThread() == thread()) {
        return job();
    }

    if (!thread()->isRunning()) {

        QuasarAppUtils::Params::log("The work threand of the async object is not running",
                                    QuasarAppUtils::Error);

        return false;
    }

    if (!await) {
        return  QMetaObject::invokeMethod(const_cast<Async*>(this),
                                          "asyncHandler",
                                          Qt::QueuedConnection,
                                          Q_ARG(QH::Async::Job, job));
    }

    bool workOfEnd = false, workResult = false;

    bool invockeResult = QMetaObject::invokeMethod(const_cast<Async*>(this),
                                                   "asyncHandler",
                                                   Qt::QueuedConnection,
                                                   Q_ARG(QH::Async::Job, job),
                                                   Q_ARG(bool*, &workOfEnd),
                                                   Q_ARG(bool*, &workResult));


    if (!invockeResult)
        return false;

    if (!waitFor(&workOfEnd, WAIT_TIME, freaze)) {
        return false;
    }

    return workResult;
}

bool Async::waitFor(const std::function<bool ()> &condition, int timeout, bool freaze) const {
    if (freaze) {
        freazePrivate(condition(), timeout)
    } else {
        waitPrivate(condition(), timeout)
    }

}

}
