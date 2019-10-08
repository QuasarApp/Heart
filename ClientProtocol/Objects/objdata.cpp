#include "objdata.h"
#include <QDataStream>

namespace ClientProtocol {

QDataStream& operator <<(QDataStream &stream, const ObjData &obj) {
    stream << obj.id;
    stream << obj.x;
    stream << obj.y;
    stream << obj.rat;

    return stream;
}

QDataStream& operator >>(QDataStream &stream, ObjData &obj) {
    stream >> obj.id;
    stream >> obj.x;
    stream >> obj.y;
    stream >> obj.rat;

    return stream;

}
}
