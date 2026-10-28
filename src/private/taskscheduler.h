//#
//# Copyright (C) 2021-2025 QuasarApp.
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
 * @brief The TaskScheduler class This class contains queue of all schedule tasks.
 * @see AbstractTask
 */
class TaskScheduler: public QObject
{
    Q_OBJECT
public:
    TaskScheduler();
    ~TaskScheduler();

    /**
     * @brief schedule This method schedule new task in this node.
     * @param task This is sharedpointe to taskObject.
     * @return true if the task scheduled successful. If the task already scheduled the return false.
     */
    bool schedule(const QSharedPointer<AbstractTask>& task);

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
     * @brief sigPushWork This signal emitted when the task @a work need to execute.
     * @param work This is needed to execute task.
     */
    void sigPushWork(QSharedPointer<QH::AbstractTask> work);

private slots:
    void handleTimeOut();

private:
    int getTimeout(qint64 timeout);


    QMultiMap<quint64, int> _taskQueue;
    QHash<int, QSharedPointer<AbstractTask>> _taskPool;
    QTimer *_timer = nullptr;
};
}
#endif // TASKSCHEDULER_H
