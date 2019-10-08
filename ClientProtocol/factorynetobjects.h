#ifndef FACTORYNETOBJECTS_H
#define FACTORYNETOBJECTS_H
#include "clientprotocol_global.h"
#include "basenetworkobject.h"
#include <QMap>
#include <typeinfo>
namespace ClientProtocol {

extern QMap<quint8, BaseNetworkObject*> map;
extern QMap<quint8, NetworkClassSize> types_sizes;

class CLIENTPROTOCOLSHARED_EXPORT FactoryNetObjects {
private:

public:
    FactoryNetObjects() = delete;

    static BaseNetworkObject *build(quint8 type);
    static BaseNetworkObject *build(quint8 type, const QByteArray& array);

    static NetworkClassSize getSize(quint8 type);
    static bool isRegisteredType(quint8 type);
    static bool isInited();

    template<typename T>
    static bool regType(quint8 id) {
        if (map.contains(id)) {
            return false;
        }

        auto tempObj = new T();
        map.insert(id, tempObj);
        types_sizes.insert(id, tempObj->classSize());
        return true;
    }
};
}


#endif // FACTORYNETOBJECTS_H
