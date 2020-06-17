/*
 * Copyright (C) 2018-2020 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
*/

#ifndef CONNECTIONINFO_H
#define CONNECTIONINFO_H

#include "abstractnodeinfo.h"
#include "accesstoken.h"
#include "networkprotocol_global.h"
#include "permisions.h"
#include <QByteArray>

class QAbstractSocket;
namespace NP {

class DbAddress;

/**
 * @brief The BaseNodeInfo class with tocken support
 */
class NETWORKPROTOCOLSHARED_EXPORT BaseNodeInfo: public AbstractNodeInfo {

public:
    bool isValid() const override;

    explicit BaseNodeInfo(QAbstractSocket * tcp = nullptr);
    ~BaseNodeInfo() override;

    /**
     * @brief permision return permision on table item of node
     * @param table name of table
     * @param id of item,
     * @return return permision
     */
    int permision(const QString& table, int id) const;

    /**
     * @brief permision - this is wraper of permision(const QString& table, int id)
     * @param address see DbAddress
     * @return permison of required object
     */
    int permision(const DbAddress& address) const;

    /**
     * @brief setPermision - set new permision for table object
     * @param table - table of set permision
     * @param id - id of object( set -1 if you need set permision for all items of table)
     * @param permision new value of permision
     */
    void setPermision(const QString& table, int id ,const Permission &permision);

    /**
     * @brief isHavePermisonRecord - check record about permision lvl of this node
     * @param table - neme of table of checked bject
     * @param id - id of checked object
     * @return true if re
     */
    bool isHavePermisonRecord(const QString& table, int id) const;


    /**
     * @brief isHavePermisonRecord - check record about permision lvl of this node
     * @param table - neme of table of checked bject
     * @param id - id of checked object
     * @return true if re
     */
    bool isHavePermisonRecord(const DbAddress& address) const;


protected:
    QHash<QString, QHash<int, Permission>> _permision;


};
}

#endif // CONNECTIONINFO_H
