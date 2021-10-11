//#
//# Copyright (C) 2021-2021 QuasarApp.
//# Distributed under the lgplv3 software license, see the accompanying
//# Everyone is permitted to copy and distribute verbatim copies
//# of this license document, but changing it is not allowed.
//#



#include "abstracttask.h"
#include <QHash>
#include <QByteArray>

namespace QH {


SheduleMode AbstractTask::mode() const {
    return _mode;
}

void AbstractTask::setMode(SheduleMode newMode) {
    _mode = newMode;
    idGen();
}

int AbstractTask::time() const {
    return _time;
}

void AbstractTask::setTime(int newTime) {
    _time = newTime;
    idGen();
}

int AbstractTask::taskId() const {
    return _taskId;
}

void AbstractTask::idGen() {
    QByteArray data;
    data.insert(0, reinterpret_cast<char*>(&_time), sizeof (_time));
    data.insert(0, reinterpret_cast<char*>(&_mode), sizeof (_mode));
    int code = typeid(this).hash_code();
    data.insert(0, reinterpret_cast<char*>(&code), sizeof (code));


    _taskId = qHash(data);
}

}
