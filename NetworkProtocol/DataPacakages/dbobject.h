/*
 * Copyright (C) 2018-2020 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
*/

#ifndef DBOBJECT_H
#define DBOBJECT_H
#include <QVariantMap>
#include <dbtablebase.h>
#include "abstractdata.h"
#include "networkprotocol_global.h"
#include "dbaddress.h"
#include "defines.h"

class QSqlQuery;

namespace NP {

/**
 * @brief The DBObject class- main class for work with data base.
 */
class NETWORKPROTOCOLSHARED_EXPORT DBObject : public AbstractData
{
public:
    /**
     * @brief DBObject
     */
    DBObject(const QString& tableName);

    ~DBObject() override;

    //// AbstractData interface
    bool isValid() const override;

    /**
     * @brief copyFrom get all data from other
     * @arg other - other data package
     * @return return true if method finished seccussful
     */
    bool copyFrom(const AbstractData * other) override;

    /**
     * @brief getId
     * @return id of objcet
     */
    int getId() const;

    /**
     * @brief getId
     * @return id of objcet
     */
    void setId(int);

    /**
     * @brief clear
     */
    virtual void clear();

    /**
     * @brief tableName
     * @return
     */
    QString tableName() const;

    /**
     * @brief setTableName set the new table for this objec
     * @param tableName
     */
    void setTableName(const QString &tableName);

    /**
     * @brief factory
     * @return self object pointer
     */
    virtual SP<DBObject> factory() = 0;

    virtual bool select(QSqlQuery& q) = 0;
    virtual bool save(QSqlQuery& q) = 0;
    virtual bool remove(QSqlQuery& q) = 0;
    virtual bool isCached() const;

    DbAddress dbAddress() const;


protected:

    QString _tableName;
    int _id = -1;

    //// StreamBase interface
    QDataStream &fromStream(QDataStream &stream) override;
    QDataStream &toStream(QDataStream &stream) const override;

};
}


#endif // DBOBJECT_H
