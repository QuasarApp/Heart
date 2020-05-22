/*
 * Copyright (C) 2018-2020 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
*/

#ifndef WORKSTATE_H
#define WORKSTATE_H

#include <QHostAddress>
#include <QList>

namespace NP {

/**
 * @brief The WorkState class
 */
class WorkState
{
private:
    int connectionCount = 0;
    int maxConnectionCount = 0;
    bool isRun = false;

    QList<QHostAddress> _banedList;

    QString getWorkStateString() const;
public:

    /**
     * @brief WorkState
     */
    WorkState();

    /**
     * @brief toString
     * @return string value of state node
     */
    QString toString() const;

    /**
     * @brief getConnectionCount
     * @return connection count on current time
     */
    int getConnectionCount() const;

    /**
     * @brief setConnectionCount set new connection count
     * @param value
     */
    void setConnectionCount(int value);

    /**
     * @brief getMaxConnectionCount
     * @return return maximum connection count
     */
    int getMaxConnectionCount() const;

    /**
     * @brief setMaxConnectionCount
     * @param value
     */
    void setMaxConnectionCount(int value);

    /**
     * @brief getBanedList
     * @return list of id's of baned nodes
     */
    QList<QHostAddress> getBanedList() const;

    /**
     * @brief setBanedList
     * @param banedList set new baned list
     */
    void setBanedList(const QList<QHostAddress> &banedList);

    /**
     * @brief getIsRun
     * @return true if noda work like server
     */
    bool getIsRun() const;

    /**
     * @brief setIsRun
     * @param value change node state
     */
    void setIsRun(bool value);
};
}
#endif // WORKSTATE_H
