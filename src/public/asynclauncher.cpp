/*
 * Copyright (C) 2021-2025 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
*/

#include "asynclauncher.h"

#include <QThread>
namespace QH {

AsyncLauncher::AsyncLauncher(QThread *thread, QObject *ptr):
    Async(thread, ptr) {
}


bool AsyncLauncher::run(const Async::Job &action, bool wait) {
    if (thread() && !thread()->isRunning()) {
        thread()->start();
    }

    return asyncLauncher(action, wait);
}

bool AsyncLauncher::stop() {
    if (thread() && thread()->isRunning()) {
        thread()->exit();
    }

    return true;
}

bool AsyncLauncher::waitForStop() {
    if (thread() && thread()->isRunning()) {
        return thread()->wait();
    }

    return true;
}

}
