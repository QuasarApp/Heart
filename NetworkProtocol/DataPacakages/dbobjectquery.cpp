#include "dbobjectquery.h"

#include <QDataStream>

namespace NP {

DBObjectQuery::DBObjectQuery(const QString& table):
    DBObject(table) {

}

short DBObjectQuery::updateIntervalSec() const {
    return _updateIntervalSec;
}

void DBObjectQuery::setUpdateIntervalSec(short updateIntervalSec) {
    _updateIntervalSec = updateIntervalSec;
}

QDataStream &DBObjectQuery::fromStream(QDataStream &stream) {
    DBObject::fromStream(stream);
    stream >> _updateIntervalSec;
    return stream;
}

QDataStream &DBObjectQuery::toStream(QDataStream &stream) const {
    DBObject::toStream(stream);
    stream << _updateIntervalSec;
    return stream;
}

bool DBObjectQuery::isCached() const {
    return false;
}

bool DBObjectQuery::isUpdateAllowed() {
    int current = static_cast<int>(time(nullptr));
    bool result = _lastUpdateSec + _updateIntervalSec < current;

    _lastUpdateSec = current;

    return result;
}
}
