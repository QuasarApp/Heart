#ifndef GLOBALDBOBJECT_H
#define GLOBALDBOBJECT_H

#include "abstractdata.h"

#include <QSet>


namespace NetworkProtocol {

class AbstractNodeInfo;

class NETWORKPROTOCOLSHARED_EXPORT GlobalDbObject: public AbstractData
{
public:

    GlobalDbObject();
    GlobalDbObject(const Package& pkg);

    int updateInterval() const;
    void setUpdateInterval(int updateInterval);

    QDataStream &fromStream(QDataStream &stream);
    QDataStream &toStream(QDataStream &stream) const;

    /**
     * @brief update - this method update globalObject
     * @return true if all good
     */
    virtual bool update() = 0;

protected:
    int _lastUpdateSce = 0;
    int _updateInterval = 60;

};
}
#endif // GLOBALDBOBJECT_H
