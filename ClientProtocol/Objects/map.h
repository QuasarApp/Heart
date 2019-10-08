#ifndef MAP_H
#define MAP_H
#include "basenetworkobject.h"
#include "objdata.h"

namespace ClientProtocol {

class CLIENTPROTOCOLSHARED_EXPORT Map : public BaseNetworkObject
{
private:
    quint8 lvl;
    quint16 lenght;
    QList<ObjData> objects;

public:
    Map();

    BaseNetworkObject *create() const override;
    NetworkClassSize classSize() const override;
    QDataStream &writeToStream(QDataStream &stream) const override;
    QDataStream &readFromStream(QDataStream &stream) override;
    bool isValid() const override;
};
}
#endif // MAP_H
