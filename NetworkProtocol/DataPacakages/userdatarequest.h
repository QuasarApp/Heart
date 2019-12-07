#ifndef USERDATAREQUEST_H
#define USERDATAREQUEST_H

#include "request.h"
#include "userdata.h"


namespace NP {

enum class UserDataRequestCmd: unsigned char {
    Invalid,
    Get,
    Login,
    Save,
    Delete
};

class NETWORKPROTOCOLSHARED_EXPORT UserDataRequest: public UserData, public Request
{
public:
    UserDataRequest();
    UserDataRequest(const Package& package);

    // StreamBase interface
    QDataStream &fromStream(QDataStream &stream);
    QDataStream &toStream(QDataStream &stream) const;

    // AbstractData interface
    bool isValid() const;

    // DBObject interface
    void clear();

};

}
#endif // USERDATAREQUEST_H
