/*
 * Copyright (C) 2018-2025 QuasarApp.
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
 * @brief The WorkState class is simple class with data of work state of node.
 */
class HEARTSHARED_EXPORT WorkState
{
public:

    /**
     * @brief WorkState
     */
    WorkState();

    /**
     * @brief toString This method convert state of work to string value.
     * @return string value of state node.
     */
    QString toString() const;

    /**
     * @brief getMaxConnectionCount This method return of limit of connections.
     * @return return maximum connection count.
     */
    int getMaxConnectionCount() const;

    /**
     * @brief setMaxConnectionCount this method set a new value of limit of connections.
     * @param value This is a new limit of connection value. 
     */
    void setMaxConnectionCount(int value);

    /**
     * @brief getBanedList This method return banned list of server.
     * @return list of id's of banned nodes.
     */
    QList<HostAddress> getBanedList() const;

    /**
     * @brief setBanedList -this method set banned list for this object.
     * @param banedList set new banned list.
     */
    void setBanedList(const QList<HostAddress> &banedList);

    /**
     * @brief getIsRun This method return true if server is runed.
     * @return true if node work like server.
     */
    bool IsRun() const;

    /**
     * @brief setIsRun This method set new value for run flag.
     * @param value change node state.
     */
    void setIsRun(bool value);

    /**
     * @brief getConnections This method return list of the current server connections.
     * @return list of the connections addresses
     */
    const QList<HostAddress>& getConnections() const;

    /**
     * @brief setConnections This method sets connections list.
     * @param connections This is a new value of the connections list.
     */
    void setConnections(const QList<HostAddress> &connections);

    /**
     * @brief activeConnections This method return list of actived connections.
     * @return list of active connections.
     */
    const QList<HostAddress> &activeConnections() const;

    /**
     * @brief setActiveConnections this method sets new list of active connections.
     * @param newActiveConnections This is new list of active connections
     */
    void setActiveConnections(const QList<HostAddress> &newActiveConnections);

private:
    int maxConnectionCount = 0;
    bool isRun = false;

    QList<HostAddress> _banedList;
    QList<HostAddress> _connections;
    QList<HostAddress> _activeConnections;

    QString getWorkStateString() const;
};
}
#endif // WORKSTATE_H
