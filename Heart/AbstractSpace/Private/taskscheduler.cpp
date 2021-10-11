//#
//# Copyright (C) 2021-2021 QuasarApp.
//# Distributed under the lgplv3 software license, see the accompanying
//# Everyone is permitted to copy and distribute verbatim copies
//# of this license document, but changing it is not allowed.
//#


#include "taskscheduler.h"

namespace QH {

TaskScheduler::TaskScheduler() {
    _timer = new  QTimer(this);

    connect(_timer, &QTimer::timeout, this, &TaskScheduler::handleTimeOut);
}

bool TaskScheduler::shedule(const QSharedPointer<AbstractTask> &task) {
    int currentTime = time(0);
    int invokeTime = 0;

    switch (task->mode()) {
    case SheduleMode::SingleWork: {
        invokeTime = currentTime + task->time();
        break;
    }
    case SheduleMode::Repeat: {
        invokeTime = currentTime + task->time();
        break;
    }
    case SheduleMode::TimePoint: {
        invokeTime = task->time();

        if (invokeTime < currentTime)
            return false;
        break;
    }
    }

    _taskPool[task->taskId()] = task;
    _taskQueue.insert(invokeTime, task->taskId());


    int top = _taskQueue.begin().key();
    int timeout = top - currentTime;
    if (timeout < 0)
        timeout = 0;

    _timer->start(timeout * 1000);

    return true;
}

bool TaskScheduler::remove(const QSharedPointer<AbstractTask> &task) {
    return remove(task->taskId());
}

bool TaskScheduler::remove(int task) {
    auto val = _taskQueue.key(task);

    _taskQueue.remove(val);
    _taskPool.remove(task);

    return true;
}

int TaskScheduler::taskCount() const {
    return _taskPool.size();
}

void TaskScheduler::handleTimeOut() {
    auto top = _taskQueue.begin();

    if (top == _taskQueue.end()) {
        _timer->stop();
        return;
    }

    auto toWork = _taskQueue.values(top.key());

    for (int key: toWork) {
        auto task = _taskPool.value(key);

        if (!task) {
            _taskPool.remove(key);
            continue;
        }

        remove(task);

        if (task->mode() == SheduleMode::Repeat) {
            shedule(task);
        }

        emit sigPushWork(task);
    }
}
}
