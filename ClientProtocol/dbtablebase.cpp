#include "dbtablebase.h"

#include <QSet>

namespace ClientProtocol {


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
