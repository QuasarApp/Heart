#ifndef GETITEM_H
#define GETITEM_H

#include <updateplayerdata.h>

namespace ClientProtocol {


class CLIENTPROTOCOLSHARED_EXPORT GetItem : public UpdatePlayerData

{
public:
    GetItem();
};

}
#endif // GETITEM_H
