/*
 * Copyright (C) 2025-2025 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
*/

#include "asyncrenderloop.h"
#include <QThread>
#include <qdebug.h>

namespace QH {

AsyncRenderLoop::AsyncRenderLoop(QThread *thread, QObject *ptr): Async(thread, ptr) {
}

AsyncRenderLoop::~AsyncRenderLoop() {

#ifdef QT_DEBUG
    Q_ASSERT_X(!isRun(), __FUNCTION__, "try to delete runned render loop! Please stop before delete."
                                       "If you the SharedPointer,"
                                       " it should be stoped monualy Or You can use AsyncRenderLoop::MainSharedPtr class");
#endif
    AsyncRenderLoop::stop();

    delete thread();

}

void QH::AsyncRenderLoop::run() {
    if (auto && thrd = thread()) {
        m_run = true;
        thrd->start();

        asyncLauncher([this](){
            renderLoopPrivate();
            return true;
        });
    }

}

void QH::AsyncRenderLoop::stop() {
    if (isRun()) {
        m_run = false;
        thread()->quit();
        thread()->wait();
    }

}

bool AsyncRenderLoop::isRun() const {
    return m_run && (thread() && thread()->isRunning());
}

void QH::AsyncRenderLoop::renderLoopPrivate() {
    auto&& currentTime = std::chrono::high_resolution_clock::now();

    _lastIterationTime = currentTime;
    int iterationTime = 0;

    while (m_run) {
        renderIteration(iterationTime);

        currentTime = std::chrono::high_resolution_clock::now();
        iterationTime = std::chrono::duration_cast<std::chrono::microseconds>(currentTime - _lastIterationTime).count();
        _lastIterationTime = currentTime;
    }
}

} // namespace QH
