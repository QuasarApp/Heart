#include "userdata.h"

#include <QDataStream>

namespace ClientProtocol {

UserData::UserData():
    DBObject("users") {

}

UserData::~UserData() {

}

QString UserData::name() const {
    return _name;
}

void UserData::setName(const QString &name) {
    _name = name;
}

QString UserData::passSHA256() const {
    return _passSHA256;
}

void UserData::setPassSHA256(const QString &passSHA256) {
    _passSHA256 = passSHA256;
}

QString UserData::mail() const {
    return _mail;
}

void UserData::setMail(const QString &mail) {
    _mail = mail;
}

int UserData::lastOnline() const {
    return _lastOnline;
}

void UserData::setLastOnline(int lastOnline) {
    _lastOnline = lastOnline;
}

int UserData::onlineTime() const {
    return _onlineTime;
}

void UserData::setOnlineTime(int onlineTime) {
    _onlineTime = onlineTime;
}

QVariantMap UserData::extraData() const {
    return _extraData;
}

void UserData::setExtraData(const QVariantMap &extraData) {
    _extraData = extraData;
}

QDataStream &UserData::fromStream(QDataStream &stream) {
    DBObject::fromStream(stream);

    stream >> _name;
    stream >> _passSHA256;
    stream >> _mail;
    stream >> _lastOnline;
    stream >> _onlineTime;
    stream >> _extraData;

    return stream;
}

QDataStream &UserData::toStream(QDataStream &stream) const {
    DBObject::toStream(stream);

    stream << _name;
    stream << _passSHA256;
    stream << _mail;
    stream << _lastOnline;
    stream << _onlineTime;
    stream << _extraData;

    return stream;
}

QVariantMap &UserData::fromVariantMap(QVariantMap &map) {
    DBObject::fromVariantMap(map);

    _name = map.value("name").toString();
    _passSHA256 = map.value("pass").toString();
    _mail = map.value("gmail").toString();
    _lastOnline = map.value("lastOnline").toInt();
    _onlineTime = map.value("onlinetime").toInt();
    _extraData = map.value("data").toMap();

    return map;
}

QVariantMap &UserData::toVariantMap(QVariantMap &map) const {
    DBObject::toVariantMap(map);

    map["name"] = _name;
    map["pass"] = _passSHA256;
    map["gmail"] = _mail;
    map["lastOnline"] = _lastOnline;
    map["onlinetime"] = _onlineTime;
    map["data"] = _extraData;

    return map;
}

void UserData::clear() {
    _name = "";
    _passSHA256 = "";
    _mail = "";
    _lastOnline = 0;
    _onlineTime = 0;
    _extraData = {};

    DBObject::clear();
}

bool UserData::isValid() const {
    return DBObject::isValid() && !_name.isEmpty();
}

}
