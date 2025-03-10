/*
 * Copyright (C) 2021-2025 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
*/

#include "asynclauncher.h"
namespace QH {

AsyncLauncher::AsyncLauncher(QThread *thread, QObject *ptr):
    Async(thread, ptr) {
}


bool AsyncLauncher::run(const Async::Job &action, bool wait) {
    return asyncLauncher(action, wait);
}

}
