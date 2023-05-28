/*
 * Copyright (C) 2018-2023 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
*/

#ifndef GETMAXINTEGERID_H
#define GETMAXINTEGERID_H

#include "dbobject.h"


namespace QH {
namespace PKG {

/**
 * @brief The GetMaxIntegerId class is intended for get a maximum value of a sql tables column.
 */
class HEARTSHARED_EXPORT GetMaxIntegerId: public DBObject
{

public:
    /**
     * @brief GetMaxIntegerId This is default constructor of the GetMaxIntegerId class.
     * @param table This is table name.
     * @param field This is name of column.
     */
    GetMaxIntegerId(const QString& table, const QString& field);

    /**
     * @brief value This method return Maximum value of a sql tables field.
     * @return Maximum value of a sql tables field.
     */
    int value() const;

    DBObject *createDBObject() const override;
    PrepareResult prepareSelectQuery(QSqlQuery &q) const override;
    bool fromSqlRecord(const QSqlRecord &q) override;
    bool isCached() const override;
    QString table() const override;

private:
    QString _table;
    QString _field;
    int _value;
};

}
}
#endif // GETMAXINTEGERID_H
