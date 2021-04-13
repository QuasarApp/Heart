/*
 * Copyright (C) 2018-2021 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
*/

#include "workstate.h"

#include <hostaddress.h>

namespace QH{

bool WorkState::IsRun() const {
    return isRun;
}

void WorkState::setIsRun(bool value) {
    isRun = value;
}

QString WorkState::getWorkStateString() const {
    if (isRun) {
        if (connectionCount >= maxConnectionCount)
            return "overload";
        else {
            return "Work";
        }
    }

    return "Not running";
}

WorkState::WorkState() {}

QString WorkState::toString() const {
    QString result("State: %0 \n"
                   "Connections: %1 / %2 \n"
                   "Baned: \n "
                   "%3");

    result = result.arg(getWorkStateString());
    result = result.arg(connectionCount).arg(maxConnectionCount);

    QString  BanedList;
    for (const auto &i: _banedList) {
         BanedList += i.toString() + "\n";
    }

    result = result.arg(BanedList);

    return result;
}

int WorkState::getConnectionCount() const {
    return connectionCount;
}

void WorkState::setConnectionCount(int value) {
    connectionCount = value;
}

int WorkState::getMaxConnectionCount() const {
    return maxConnectionCount;
}

void WorkState::setMaxConnectionCount(int value) {
    maxConnectionCount = value;
}

QList<HostAddress> WorkState::getBanedList() const {
    return _banedList;
}

void WorkState::setBanedList(const QList<HostAddress> &banedList) {
    _banedList = banedList;
}
}

