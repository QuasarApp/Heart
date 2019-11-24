#include "userdata.h"

#include <QDataStream>
#include <QSharedPointer>
#include <QSqlQuery>

namespace NetworkProtocol {

UserData::UserData():
    DBObject("users") {

}

UserData::UserData(const Package &package): DBObject("users") {
    fromBytes(package.data);
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

QVariantMap &UserData::extraData() {
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
    stream >> _token;

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
    stream << _token;

    return stream;
}

bool UserData::select(QSqlQuery &q) {
    QString query;

    if (getId() > 0) {
        query = "SELECT * from '" + tableName() +
        "' where id='" + QString::number(getId()) + "'";
    } else {
        query = "SELECT * from '" + tableName() +
        "' where gmail='" + QString::number(getId()) + "'";
    }

    if (!q.prepare(query))
        return false;

    if (!q.exec())
        return false;

    if (!q.next())
        return false;

    setId(q.value("id").toInt());
    _name = q.value("name").toString();
    _passSHA256 = q.value("pass").toString();
    _mail = q.value("gmail").toString();
    _lastOnline = q.value("lastOnline").toString();
    _onlineTime = q.value("onlinetime").toString();
    _name = q.value("points").toString();
    _name = q.value("data").toString();

    return isValid();

}

bool UserData::save(QSqlQuery &q) {

}

bool UserData::remove(QSqlQuery &q) {

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
    return DBObject::isValid() && _mail.size();
}

QSharedPointer<DBObject> UserData::factory() {
    return QSharedPointer<UserData>::create();
}

const AccessToken &UserData::token() const {
    return _token;
}

void UserData::setToken(const AccessToken &token) {
    _token = token;
}

int UserData::points() const
{
    return _points;
}

void UserData::setPoints(int points)
{
    _points = points;
}

}
