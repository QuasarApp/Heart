/*
 * Copyright (C) 2018-2020 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
*/


#ifndef NODEOBJECT_H
#define NODEOBJECT_H

#include "dbobject.h"


namespace NP {

/**
 * @brief The NodeObject class - database structure of node
 */
class NETWORKPROTOCOLSHARED_EXPORT NodeObject: public DBObject
{
public:
    NodeObject();
    NodeObject(const Package& pkg);

    // DBObject interface
    DBObject *factory() override;
    bool select(QSqlQuery &q) override;
    bool save(QSqlQuery &q) const override;

private:
    QByteArray _publickKey;
};
}
#endif // NODEOBJECT_H
