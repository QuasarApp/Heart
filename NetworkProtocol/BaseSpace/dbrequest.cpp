#include "dbrequest.h"
namespace NP {

DBRequest::DBRequest()
{

}

DbAddress DBRequest::address() const
{
    return _address;
}

void DBRequest::setAddress(const DbAddress &address)
{
    _address = address;
}
}
