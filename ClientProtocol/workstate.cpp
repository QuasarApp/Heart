#include "workstate.h"

#include <QHostAddress>

namespace ClientProtocol{

bool WorkState::getIsRun() const {
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
    for (auto i: _banedList) {
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

QList<QHostAddress> WorkState::getBanedList() const {
    return _banedList;
}

void WorkState::setBanedList(const QList<QHostAddress> &banedList) {
    _banedList = banedList;
}
}

