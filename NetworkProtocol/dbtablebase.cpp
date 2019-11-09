#include "dbtablebase.h"

#include <QSet>

namespace NetworkProtocol {


QString DbTableBase::lastIdQuery() const {
    return QString("SELECT MAX(id) FROM " + name);
}

bool DbTableBase::isInited() const {
    return keys.size();
}

bool DbTableBase::isValid() const {
    return name.size();
}

}
