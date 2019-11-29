#ifndef RATINGTABLE_H
#define RATINGTABLE_H

#include "dbobjectquery.h"
#include "userrecord.h"


namespace NetworkProtocol {

class NETWORKPROTOCOLSHARED_EXPORT RatingTable : public DBObjectQuery
{
public:
    RatingTable();
    RatingTable(const Package& package);

    QDataStream &fromStream(QDataStream &stream);
    QDataStream &toStream(QDataStream &stream) const;

    QSharedPointer<DBObject> factory();
    bool select(QSqlQuery &q);
    bool save(QSqlQuery &q);
    bool remove(QSqlQuery &q);

    bool isValid() const;
    void clear();

private:
    QMap<short, UserRecord> _data;

};
}
#endif // RATINGTABLE_H
