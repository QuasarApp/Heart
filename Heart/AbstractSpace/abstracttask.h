//#
//# Copyright (C) 2021-2021 QuasarApp.
//# Distributed under the lgplv3 software license, see the accompanying
//# Everyone is permitted to copy and distribute verbatim copies
//# of this license document, but changing it is not allowed.
//#

#ifndef ABSTRACTTASK_H
#define ABSTRACTTASK_H
#include <QSharedPointer>

namespace QH {

class AbstractNode;
/**
 * @brief The SheduleMode enum contails list of the shedule modes.
 */
enum class SheduleMode {
    /// In this mode AbstractTask will be executed after **time** secunds from the moment of adding this task.
    SingleWork,
    /// In this mode AbstractTask will be executed task every **time** from the moment of adding this task.
    Repeat,
    /// In this mode AbstractTask will be executed int **time** secunds by Unix time.
    TimePoint
};

/**
 * @brief The AbstractTask class. All tasks executed on separate thread.
 */
class AbstractTask
{
public:
    AbstractTask() = default;
    virtual ~AbstractTask() = default;

    /**
     * @brief mode This method retunr current mode of this task.
     * @return current mode of this task.
     * @see AbstractTask::setMode
     */
    SheduleMode mode() const;

    /**
     * @brief setMode This method sets new mode of this task.
     * @param newMode new mode of this task.
     * @see AbstractTask::mode
     */
    void setMode(SheduleMode newMode);

    /**
     * @brief time This is universal property.
     * This property has its own meaning for each AbstractTask::mode.
     *  For more information see the SheduleMode enum.
     * @return time property.
     * @see AbstractTask::setTime
     */
    int time() const;

    /**
     * @brief setTime This method sets new value for the AbstractTask::time property.
     * @param newTime This is new value of the time propertye.
     * @see AbstractTask::time
     */
    void setTime(int newTime);

    /**
     * @brief execute This method will be invoked when task be executed.
     * @param node This is pointer to node object.
     * @return true if the work winished successfull
     */
    virtual bool execute(AbstractNode *node) const = 0;

    /**
     * @brief taskId This method return id of this task.
     * @return id of this task.
     */
    int taskId() const;

private:
    void idGen();
    SheduleMode _mode = SheduleMode::SingleWork;
    int _time = 0;
    int _taskId = 0;
};

}

Q_DECLARE_METATYPE(QSharedPointer<QH::AbstractTask>)


#endif // ABSTRACTTASK_H
