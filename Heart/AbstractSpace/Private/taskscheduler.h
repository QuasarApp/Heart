//#
//# Copyright (C) 2021-2021 QuasarApp.
//# Distributed under the lgplv3 software license, see the accompanying
//# Everyone is permitted to copy and distribute verbatim copies
//# of this license document, but changing it is not allowed.
//#

#ifndef TASKSCHEDULER_H
#define TASKSCHEDULER_H

#include "abstracttask.h"

#include <QHash>
#include <QMap>
#include <QSharedPointer>
#include <QTimer>

namespace QH {

/**
 * @brief The TaskScheduler class This class contains queue of all shedule tasks.
 * @see AbstractTask
 */
class TaskScheduler: public QObject
{
    Q_OBJECT
public:
    TaskScheduler();
    ~TaskScheduler();

    /**
     * @brief shedule This method shedule new task in this node.
     * @param task This is sharedpointe to taskObject.
     * @return true if the task sheduled successful. If the task alredy sheduled the return false.
     */
    bool shedule(const QSharedPointer<AbstractTask>& task);

    /**
     * @brief remove This method remove the @a task from a tasks queue.
     * @param task This is removed task.
     * @return true if the task remove successful
     */
    bool remove(const QSharedPointer<AbstractTask>& task);

    /**
     * @brief remove This method remove the task with @a task id.
     * @param task This is id of the removed task.
     * @return true if the method removed successful
     */
    bool remove(int task);

    /**
     * @brief taskCount This method return tasks count.
     * @return tasks count
     */
    int taskCount() const;

signals:
    /**
     * @brief sigPushWork This signal emited when the task @a work neet to execute.
     * @param work This is needed to execute task.
     */
    void sigPushWork(QSharedPointer<QH::AbstractTask> work);

private slots:
    void handleTimeOut();

private:
    QMultiMap<quint64, int> _taskQueue;
    QHash<int, QSharedPointer<AbstractTask>> _taskPool;
    QTimer *_timer = nullptr;
};
}
#endif // TASKSCHEDULER_H
