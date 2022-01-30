#include "singleserverdb.h"

namespace QH {

SingleServerDB::SingleServerDB()
{

}

ISqlDBCache *QH::SingleServerDB::rawDb() const {
    return DataBase::db();
}

QStringList SingleServerDB::SQLSources() const {
    return {":/sql/DataBaseSpace/Res/UserDB.sql"};
}

}
