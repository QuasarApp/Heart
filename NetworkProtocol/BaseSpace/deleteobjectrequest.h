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

    DBObject *factory() const override;
    bool prepareSelectQuery(QSqlQuery &q) const override;
    bool prepareSaveQuery(QSqlQuery &q) const override;
    bool prepareRemoveQuery(QSqlQuery &q) const override;
};
}
#endif // DELETEOBJECTREQUEST_H
