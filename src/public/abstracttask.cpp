//#
//# Copyright (C) 2021-2025 QuasarApp.
//# Distributed under the lgplv3 software license, see the accompanying
//# Everyone is permitted to copy and distribute verbatim copies
//# of this license document, but changing it is not allowed.
//#



#include "abstracttask.h"
#include <QHash>
#include <QByteArray>

namespace QH {


ScheduleMode AbstractTask::mode() const {
    return _mode;
}

void AbstractTask::setMode(ScheduleMode newMode) {
    _mode = newMode;
    idGen();
}

quint64 AbstractTask::time() const {
    return _time;
}

void AbstractTask::setTime(quint64 newTime) {
    _time = newTime;
    idGen();
}

int AbstractTask::taskId() const {
    return _taskId;
}

bool AbstractTask::isValid() const {
    switch (_mode) {
    case ScheduleMode::Repeat: {
        return _time > 1000 && _taskId;
    }
    default: return _taskId;
    }
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
