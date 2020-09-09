#include "networkrequest.h"

#include <QDataStream>
namespace NP {

NetworkRequest::NetworkRequest() {

}

NetworkRequest::NetworkRequest(const Package &package):
    NetworkRequest() {

    fromBytes(package.data);
}

bool NetworkRequest::isValid() const {

}

bool NetworkRequest::copyFrom(const AbstractData *) {

}

QString NetworkRequest::toString() const {

}

Package NetworkRequest::dataRequest() const {
    return _dataRequest;
}

void NetworkRequest::setDataRequest(const Package &data) {
    _dataRequest = data;
}

Package NetworkRequest::dataResponce() const {
    return _dataResponce;
}

void NetworkRequest::setDataResponce(const Package &data) {
    _dataResponce = data;
}

bool NetworkRequest::isComplete() const {
    return _dataResponce.isValid();
}

void NetworkRequest::addNodeRequiringData(const BaseId &node) {
    _askedNodes.insert(node);
}

QDataStream &NetworkRequest::fromStream(QDataStream &stream) {
    AbstractData::fromStream(stream);

    stream >> _dataRequest;
    stream >> _askedNodes;
    stream >> _dataResponce;
    return stream;

}

QDataStream &NetworkRequest::toStream(QDataStream &stream) const {
    AbstractData::toStream(stream);

    stream << _dataRequest;
    stream << _askedNodes;
    stream << _dataResponce;

    return stream;
}

bool NetworkRequest::init() {

}

QSet<BaseId> NetworkRequest::askedNodes() const {
    return _askedNodes;
}

void NetworkRequest::removeNodeFromAskedList(const BaseId &node) {
    _askedNodes.remove(node);
}


}
