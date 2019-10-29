#ifndef USERDATA_H
#define USERDATA_H

#include "dbobject.h"

namespace ClientProtocol {

/**
 * @brief The UserData class - is default implementation of users items of database
 */
class CLIENTPROTOCOLSHARED_EXPORT UserData : public DBObject
{
public:
    UserData();
    ~UserData();
};

}

#endif // USERDATA_H
