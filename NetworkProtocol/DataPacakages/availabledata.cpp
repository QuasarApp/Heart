#include "availabledata.h"

#include <QDataStream>

namespace NetworkProtocol {

AvailableData::AvailableData() {
    generateId<decltype (this)>();
}


AvailableData::AvailableData(const Package &pkg):AbstractData(pkg) {
    generateId<AvailableData>();
}

QDataStream &AvailableData::fromStream(QDataStream &stream) {
    AbstractData::fromStream(stream);
    stream >> _data;
    return stream;
}

QDataStream &AvailableData::toStream(QDataStream &stream) const {
    AbstractData::toStream(stream);
    stream << _data;
    return stream;
}

bool AvailableData::isValid() const {
    return AbstractData::isValid() && _data.size();
}

QHash<QString, QList<int> > AvailableData::data() const {
    return _data;
}

void AvailableData::setData(const QHash<QString, QList<int> > &data) {
    _data = data;
}

QList<int>& AvailableData::operator[](const QString &key) {
    return _data[key];
}

}

