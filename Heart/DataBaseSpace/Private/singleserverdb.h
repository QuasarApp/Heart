#ifndef SINGLESERVERDB_H
#define SINGLESERVERDB_H

#include <database.h>

namespace QH {


class SingleServerDB: public DataBase
{
    Q_OBJECT
public:
    SingleServerDB();
    /**
     * @brief rawDb This node return pointer to database object.
     * @return The pointer to data base.
     */
    ISqlDBCache* rawDb() const;

    QStringList SQLSources() const override;

    friend class SingleServer;
};

}
#endif // SINGLESERVERDB_H
