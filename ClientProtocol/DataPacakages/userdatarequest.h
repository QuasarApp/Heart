#ifndef USERDATAREQUEST_H
#define USERDATAREQUEST_H

#include "userdata.h"


namespace ClientProtocol {

enum class UserDataRequestCmd: unsigned char {
    Invalid,
    Get,
    Login,
    Save,
    Delete
};

class CLIENTPROTOCOLSHARED_EXPORT UserDataRequest: public UserData
{
public:
    UserDataRequest();
    UserDataRequest(const Package& package);

    UserDataRequestCmd requestCmd() const;
    void setRequestCmd(const UserDataRequestCmd &requestCmd);

    // StreamBase interface
    QDataStream &fromStream(QDataStream &stream);
    QDataStream &toStream(QDataStream &stream) const;

    // AbstractData interface
    bool isValid() const;

    // DBObject interface
    void clear();

private:
    UserDataRequestCmd _requestCmd = UserDataRequestCmd::Invalid;

};

}
#endif // USERDATAREQUEST_H
