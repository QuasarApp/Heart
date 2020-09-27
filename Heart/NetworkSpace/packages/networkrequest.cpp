/*
 * Copyright (C) 2018-2020 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
*/


#include "networkrequest.h"

#include <QDataStream>
namespace QH {
namespace PKG {

NetworkRequest::NetworkRequest() {

}

NetworkRequest::NetworkRequest(const Package &package):
    NetworkRequest() {

    fromBytes(package.data);
}

bool NetworkRequest::isValid() const {
    return AbstractData::isValid() && _askedNodes.size() && _dataRequest.isValid();
}

bool NetworkRequest::copyFrom(const AbstractData * other) {
    if (!AbstractData::copyFrom(other))
        return false;

    auto otherObject = dynamic_cast<const NetworkRequest*>(other);
    if (!otherObject)
        return false;

    this->_dataRequest = otherObject->_dataRequest;
    this->_askedNodes = otherObject->_askedNodes;
    this->_dataResponce = otherObject->_dataResponce;

    return true;
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

QSet<BaseId> NetworkRequest::askedNodes() const {
    return _askedNodes;
}

void NetworkRequest::removeNodeFromAskedList(const BaseId &node) {
    _askedNodes.remove(node);
}


}
}
