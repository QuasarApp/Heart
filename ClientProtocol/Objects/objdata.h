#ifndef OBJDATA_H
#define OBJDATA_H
#include "clientprotocol_global.h"
namespace ClientProtocol {

struct CLIENTPROTOCOLSHARED_EXPORT ObjData {
    int id;
    unsigned short x;
    unsigned short y;
    unsigned short rat;

    friend QDataStream& operator << (QDataStream& stream, const ObjData& obj);
    friend QDataStream& operator >> (QDataStream& stream, ObjData& obj);

};
}
#endif // OBJDATA_H
