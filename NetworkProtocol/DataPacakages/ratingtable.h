#ifndef RATINGTABLE_H
#define RATINGTABLE_H

#include "dbobjectquery.h"
#include "userrecord.h"


namespace NP {

class NETWORKPROTOCOLSHARED_EXPORT RatingTable : public DBObjectQuery
{
public:
    RatingTable();
    RatingTable(const Package& package);

    QDataStream &fromStream(QDataStream &stream);
    QDataStream &toStream(QDataStream &stream) const;

    SP<DBObject> factory();
    bool select(QSqlQuery &q);
    bool save(QSqlQuery &q);
    bool remove(QSqlQuery &q);

    bool isValid() const;
    void clear();

    const QMap<short, UserRecord> &data() const;

    static QString table;
    static int id;
    static DbAddress getDBAddress();

private:
    QMap<short, UserRecord> _data;

};
}
#endif // RATINGTABLE_H
