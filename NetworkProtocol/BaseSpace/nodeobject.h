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
