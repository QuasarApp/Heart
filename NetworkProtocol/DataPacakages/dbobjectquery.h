#ifndef DBOBJECTQUERY_H
#define DBOBJECTQUERY_H

#include "dbobject.h"

namespace NetworkProtocol {

class NETWORKPROTOCOLSHARED_EXPORT DBObjectQuery : public DBObject
{
public:
    DBObjectQuery(const QString& table);

    short updateIntervalSec() const;
    void setUpdateIntervalSec(short updateIntervalSec);

    QDataStream &fromStream(QDataStream &stream);
    QDataStream &toStream(QDataStream &stream) const;

    bool isCached() const;

protected:
    bool isUpdateAllowed();

private:
    short _updateIntervalSec = 0;
    int _lastUpdateSec = 0;


};

}
#endif // DBOBJECTQUERY_H
