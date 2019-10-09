#include "basenodeinfo.h"
#include <QTcpSocket>
#include <QHostAddress>

namespace ClientProtocol {


int BaseNodeInfo::getKarma() const {
    return karma;
}

void BaseNodeInfo::setKarma(int value) {
    karma = value;

    if (isBaned()) {
        disconnect();
    }
}

QAbstractSocket *BaseNodeInfo::getSct() const {
    return _sct;
}

void BaseNodeInfo::setSct(QAbstractSocket *value) {
    _sct = value;
}

QByteArray BaseNodeInfo::getToken() const {
    return token;
}

void BaseNodeInfo::setToken(const QByteArray &value) {
    token = value;
}

Relations BaseNodeInfo::getRel() const {
    return _rel;
}

void BaseNodeInfo::setRel(const Relations &rel) {
    _rel = rel;
}

void BaseNodeInfo::disconnect() {
    if (_sct) {
        _sct->close();
        token = "";
        _sct->deleteLater();
        _sct = nullptr;
    }
}

unsigned int BaseNodeInfo::id() const {
    if (_sct)
        return (qHash(_sct->peerAddress()));

    return 0;
}

void BaseNodeInfo::ban() {
    karma = BANED_KARMA;
    disconnect();
}

bool BaseNodeInfo::isBaned() const {
    return karma < 1;
}

void BaseNodeInfo::unBan() {
    karma = RESTORE_KARMA;
}

bool BaseNodeInfo::isValid() const {
    return _sct;
}

BaseNodeInfo::BaseNodeInfo(QAbstractSocket *tcp, int kar) {
    _sct = tcp;
    karma = kar;
    token = "";
}

BaseNodeInfo::~BaseNodeInfo() {
    if (_sct) {
        _sct->deleteLater();
    }
}
}
