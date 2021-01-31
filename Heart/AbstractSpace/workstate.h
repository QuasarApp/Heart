/*
 * Copyright (C) 2018-2021 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
*/

#ifndef WORKSTATE_H
#define WORKSTATE_H

#include <hostaddress.h>
#include <QList>

namespace QH {

/**
 * @brief The WorkState class is simple class with data of work statte of node.
 */
class WorkState
{
public:

    /**
     * @brief WorkState
     */
    WorkState();

    /**
     * @brief toString This method convert state of owrk to string value
     * @return string value of state node
     */
    QString toString() const;

    /**
     * @brief getConnectionCount return connections count of node
     * @return connection count on current time
     */
    int getConnectionCount() const;

    /**
     * @brief setConnectionCount set new connection count
     * @param value This is new value of connections count.
     */
    void setConnectionCount(int value);

    /**
     * @brief getMaxConnectionCount This method return of limit of connections
     * @return return maximum connection count
     */
    int getMaxConnectionCount() const;

    /**
     * @brief setMaxConnectionCount this method set a new value of limit of connections
     * @param value
     */
    void setMaxConnectionCount(int value);

    /**
     * @brief getBanedList This methd return banned list of server
     * @return list of id's of baned nodes
     */
    QList<HostAddress> getBanedList() const;

    /**
     * @brief setBanedList -this method set banned list for this object
     * @param banedList set new baned list
     */
    void setBanedList(const QList<HostAddress> &banedList);

    /**
     * @brief getIsRun This method return true if server is runed
     * @return true if noda work like server
     */
    bool IsRun() const;

    /**
     * @brief setIsRun This method set new value for run flag.
     * @param value change node state
     */
    void setIsRun(bool value);

private:
    int connectionCount = 0;
    int maxConnectionCount = 0;
    bool isRun = false;

    QList<HostAddress> _banedList;

    QString getWorkStateString() const;
};
}
#endif // WORKSTATE_H
