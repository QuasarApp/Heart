#include "userbasedata.h"

#include <QDataStream>
#include <QSharedPointer>
#include <QSqlQuery>
#include <quasarapp.h>
#include <QSqlError>

namespace NP {

UserBaseData::UserBaseData(): DBObject("Users") {
    clear();
    INIT_COMMAND
}

SP<DBObject> UserBaseData::factory() {
    return SP<UserBaseData>::create();
}

bool UserBaseData::select(QSqlQuery &q) {
    QString query;

    if (getId() > 0) {
        query = "SELECT * from '" + tableName() +
        "' where id='" + QString::number(getId()) + "'";
    } else {
        query = "SELECT * from '" + tableName() +
        "' where userName='" + _name + "'";
    }

    if (!q.prepare(query))
        return false;

    if (!q.exec())
        return false;

    if (!q.next())
        return false;

    setId(q.value("id").toInt());
    _name = q.value("userName").toString();

    auto array = q.value("pass").toByteArray();
    QDataStream s(&array, QIODevice::ReadWrite);
    s >> _passSHA256;

    return isValid();
}

bool UserBaseData::save(QSqlQuery &q) {
    QString queryString = "INSERT INTO %0(%1) VALUES (%2)";

    queryString = queryString.arg(tableName());

    queryString = queryString.arg(
                "userName, pass");

    QString values;

    values += "'" + _name + "', ";
    values += "'" + _passSHA256 + "', ";

    queryString = queryString.arg(values);

    if (!q.prepare(queryString)) {
        QuasarAppUtils::Params::log("prepare sql query error: " + q.lastError().text() + " Driver: " + q.lastError().driverText(),
                                    QuasarAppUtils::Debug);
        return false;
    }

    if (!q.exec()) {
        QuasarAppUtils::Params::log("exec sql query error: " + q.lastError().text() + " Driver: " + q.lastError().driverText(),
                                    QuasarAppUtils::Debug);
        return false;
    }

    return true;
}

QString UserBaseData::name() const {
    return _name;
}

void UserBaseData::setName(const QString &name) {
    _name = name;
}

QByteArray UserBaseData::passSHA256() const {
    return _passSHA256;
}

void UserBaseData::setPassSHA256(const QByteArray &passSHA256) {
    _passSHA256 = passSHA256;
}

bool UserBaseData::copyFrom(const AbstractData * other) {
    if (!DBObject::copyFrom(other))
        return false;

    auto otherUser = dynamic_cast<const UserBaseData*>(other);
    if (!otherUser)
        return false;

    this->_name = otherUser->_name;
    this->_passSHA256 = otherUser->_passSHA256;

    return true;
}

bool UserBaseData::remove(QSqlQuery &q) {
    return DBObject::remove(q);
}

bool UserBaseData::isValid() const {
    return DBObject::isValid() && _name.size() &&
            (_passSHA256.size() == 32 || _token.isValid());
}

void UserBaseData::clear() {
    _name = "";
    _passSHA256 = "";
    _token.clear();

    DBObject::clear();
}

QDataStream &UserBaseData::fromStream(QDataStream &stream) {
    return stream;
}

QDataStream &UserBaseData::toStream(QDataStream &stream) const {
    return stream;
}

AccessToken UserBaseData::token() const {
    return _token;
}

void UserBaseData::setToken(const AccessToken &token) {
    _token = token;
}

}
