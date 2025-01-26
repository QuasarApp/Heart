/*
 * Copyright (C) 2025-2025 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
*/

#include "asyncrenderloop.h"
#include <QDateTime>
#include <QThread>

namespace QH {

AsyncRenderLoop::AsyncRenderLoop(QThread *thread, QObject *ptr): Async(thread, ptr) {

}

AsyncRenderLoop::~AsyncRenderLoop() {
    AsyncRenderLoop::stop();
}

void QH::AsyncRenderLoop::run() {
    m_run = true;
    asyncLauncher([this](){
        renderLoopPrivate();
        return true;
    });
}

void QH::AsyncRenderLoop::stop() {
    m_run = false;
    thread()->quit();
    thread()->wait();
}

bool AsyncRenderLoop::isRun() const {
    return m_run || (thread() && thread()->isRunning());
}

void QH::AsyncRenderLoop::renderLoopPrivate() {
    quint64 currentTime = QDateTime::currentMSecsSinceEpoch();

    _lastIterationTime = currentTime;
    int iterationTime = 0;

    while (m_run) {
        renderIteration(iterationTime);

        currentTime = QDateTime::currentMSecsSinceEpoch();
        iterationTime = currentTime - _lastIterationTime;
        _lastIterationTime = currentTime;
    }
}

} // namespace QH
