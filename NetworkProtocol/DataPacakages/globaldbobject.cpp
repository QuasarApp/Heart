#include "globaldbobject.h"
namespace NetworkProtocol {

GlobalDbObject::GlobalDbObject()
{

}

GlobalDbObject::GlobalDbObject(const Package &pkg):AbstractData(pkg) {

}

int GlobalDbObject::updateInterval() const
{
    return _updateInterval;
}

void GlobalDbObject::setUpdateInterval(int updateInterval)
{
    _updateInterval = updateInterval;
}

QDataStream &GlobalDbObject::fromStream(QDataStream &stream) {

}

QDataStream &GlobalDbObject::toStream(QDataStream &stream) const {

}

}
