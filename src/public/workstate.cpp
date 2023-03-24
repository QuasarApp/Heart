/*
 * Copyright (C) 2018-2023 QuasarApp.
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

const QList<HostAddress> &WorkState::getConnections() const {
    return _connections;
}

void WorkState::setConnections(const QList<HostAddress> &connections) {
    _connections = connections;
}

const QList<HostAddress> &WorkState::activeConnections() const {
    return _activeConnections;
}

void WorkState::setActiveConnections(const QList<HostAddress> &newActiveConnections) {
    _activeConnections = newActiveConnections;
}

QString WorkState::getWorkStateString() const {
    if (isRun) {
        if (_activeConnections.size() >= maxConnectionCount)
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
                   "Connections: %1 \n"
                   "Actived connections: %2 / %3 \n"
                   "Connected Addresses:\n"
                   "%4 \n"
                   "Baned: \n "
                   "%5");

    result = result.arg(getWorkStateString()).
            arg(_connections.size()).
            arg(_activeConnections.size()).
            arg(maxConnectionCount);

    QString  connections;
    for (const auto &i: _activeConnections) {
         connections += i.toString() + "\n";
    }

    result = result.arg(connections);

    QString  BanedList;
    for (const auto &i: _banedList) {
         BanedList += i.toString() + "\n";
    }

    result = result.arg(BanedList);

    return result;
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

