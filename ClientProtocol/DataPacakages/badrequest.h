#ifndef BADREQUEST_H
#define BADREQUEST_H

#include "abstractdata.h"

namespace ClientProtocol{

class BadRequest : public AbstractData
{
public:
    explicit BadRequest();
    explicit BadRequest(const Package& package);

};

}

#endif // BADREQUEST_H
