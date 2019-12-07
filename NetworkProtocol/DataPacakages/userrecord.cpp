#include "userrecord.h"

#include <QDataStream>

namespace NP {


UserRecord::UserRecord()
{
    INIT_COMMAND
}

UserRecord::UserRecord(const Package &package):
    UserRecord() {
    fromBytes(package.data);
}

QDataStream &NP::UserRecord::fromStream(QDataStream &stream) {
    AbstractData::fromStream(stream);
    stream >> name;
    stream >> record;
    return stream;
}

QDataStream &NP::UserRecord::toStream(QDataStream &stream) const {
    AbstractData::toStream(stream);
    stream << name;
    stream << record;
    return stream;
}

bool UserRecord::isValid() const {
    return !name.isEmpty() && record > 0 && AbstractData::isValid();
}

QString UserRecord::getName() const {
    return name;
}

void UserRecord::setName(const QString &value) {
    name = value;
}

short UserRecord::getRecord() const {
    return record;
}

void UserRecord::setRecord(short value) {
    record = value;
}

}
