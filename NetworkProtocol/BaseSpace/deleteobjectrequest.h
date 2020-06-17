#ifndef DELETEOBJECTREQUEST_H
#define DELETEOBJECTREQUEST_H
#include <dbobject.h>
namespace NP {

/**
 * @brief The DeleteObjectRequest class general request for remove object from db
 */
class NETWORKPROTOCOLSHARED_EXPORT DeleteObjectRequest: public DBObject
{
public:
    explicit DeleteObjectRequest(const Package& pkg);
    explicit DeleteObjectRequest(const DbAddress& address);;

    DBObject *factory();
    bool select(QSqlQuery &q);
    bool save(QSqlQuery &q) const;
    bool remove(QSqlQuery &q) const;
};
}
#endif // DELETEOBJECTREQUEST_H
