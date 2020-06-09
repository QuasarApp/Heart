#ifndef DELETEOBJECTREQUEST_H
#define DELETEOBJECTREQUEST_H
#include "dbrequest.h"

#include <abstractdata.h>
namespace NP {

/**
 * @brief The DeleteObjectRequest class general request for remove object from db
 */
class NETWORKPROTOCOLSHARED_EXPORT DeleteObjectRequest: public AbstractData, public DBRequest
{
public:
    explicit DeleteObjectRequest();
    explicit DeleteObjectRequest(const DbAddress& address);

};
}
#endif // DELETEOBJECTREQUEST_H
