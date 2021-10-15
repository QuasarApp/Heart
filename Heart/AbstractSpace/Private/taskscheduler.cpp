//#
//# Copyright (C) 2021-2021 QuasarApp.
//# Distributed under the lgplv3 software license, see the accompanying
//# Everyone is permitted to copy and distribute verbatim copies
//# of this license document, but changing it is not allowed.
//#


#include "taskscheduler.h"
#include <QDateTime>

namespace QH {

TaskScheduler::TaskScheduler() {
    _timer = new  QTimer();

    connect(_timer, &QTimer::timeout, this, &TaskScheduler::handleTimeOut);
}

TaskScheduler::~TaskScheduler() {
    _timer->stop();
    delete _timer;
}

int QH::TaskScheduler::getTimeout(qint64 timeout) {
    return std::max(std::min(static_cast<qint64>(AbstractTask::Day),
                             timeout), static_cast<qint64>(0));
}

bool TaskScheduler::shedule(const QSharedPointer<AbstractTask> &task) {
    if (!task->isValid())
        return false;

    quint64 currentTime = QDateTime::currentMSecsSinceEpoch();
    quint64 invokeTime = 0;

    switch (task->mode()) {
    case ScheduleMode::SingleWork: {
        invokeTime = currentTime + task->time();
        break;
    }
    case ScheduleMode::Repeat: {
        invokeTime = currentTime + task->time();
        break;
    }
    case ScheduleMode::TimePoint: {
        invokeTime = task->time();

        if (invokeTime < currentTime)
            return false;
        break;
    }
    }

    _taskPool[task->taskId()] = task;
    _taskQueue.insert(invokeTime, task->taskId());


    qint64 top = _taskQueue.begin().key();
    qint64 timeout = top - currentTime;

    _timer->start(getTimeout(timeout));

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

    unsigned long long currentTime = QDateTime::currentMSecsSinceEpoch();
    if (top.key() > currentTime) {
        _timer->start(getTimeout(top.key() - currentTime));
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

        if (task->mode() == ScheduleMode::Repeat) {
            shedule(task);
        }

        emit sigPushWork(task);
    }
}
}
