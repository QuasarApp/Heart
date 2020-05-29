#include "userpermision.h"
#include <QDataStream>
#include <QSharedPointer>
#include <QSqlQuery>
#include <quasarapp.h>
#include <QSqlError>


namespace NP {

UserPermision::UserPermision():
    DBObject("UserPermisions") {

}

SP<DBObject> UserPermision::factory() {
    return SP<UserPermision>::create();
}

bool UserPermision::copyFrom(const AbstractData * other) {
    if (!DBObject::copyFrom(other))
        return false;

    auto otherUser = dynamic_cast<const UserPermision*>(other);
    if (!otherUser)
        return false;

    this->_data = otherUser->_data;
    return true;
}

bool UserPermision::remove(QSqlQuery &q) {
    if (!DBObject::remove(q)) {

        if (!_data.size()) {
            return false;
        }

        if (_data.size() && _data.size()) {
            QuasarAppUtils::Params::log("UserPermision object contains multiple items." + QString(Q_FUNC_INFO),
                                        QuasarAppUtils::Warning);
        }

        QString query;

        query = "DELETE from '" + tableName() + "' ";

        QString where;

        auto key = _data.begin().key();

        if (key._userId) {
            where += QString((where.isEmpty())? "" : ",") + " userId=" + QString::number(key._userId) + "";
        }

        if (!key._objectTable.isEmpty()) {
            where += QString((where.isEmpty())? "" : ",") + " objectTable='" + key._objectTable + "'";
        }

        if (key._idObject) {
            where += QString((where.isEmpty())? "" : ",") + " objectId=" + QString::number(key._idObject) + "";
        }

        query += where;

        if (!q.prepare(query))
            return false;

        return q.exec();
    }

    return true;
}

void UserPermision::clear() {
    _data.clear();

    DBObject::clear();
}

bool UserPermision::isValid() const {
    return _data.size();
}

bool UserPermision::select(QSqlQuery &q) {

    if (!_data.size()) {
        return false;
    }

    if (_data.size() && _data.size()) {
        QuasarAppUtils::Params::log("UserPermision object contains multiple items." + QString(Q_FUNC_INFO),
                                    QuasarAppUtils::Warning);
    }

    QString query;

    query = "SELECT * from '" + tableName() + "' ";

    QString where;

    auto key = _data.begin().key();

    if (key._userId) {
        where += QString((where.isEmpty())? "" : ",") + " userId=" + QString::number(key._userId) + "";
    }

    if (!key._objectTable.isEmpty()) {
        where += QString((where.isEmpty())? "" : ",") + " objectTable='" + key._objectTable + "'";
    }

    if (key._idObject) {
        where += QString((where.isEmpty())? "" : ",") + " objectId=" + QString::number(key._idObject) + "";
    }

    query += where;

    if (!q.prepare(query))
        return false;

    if (!q.exec())
        return false;

    while (q.next()) {
        PermisionData data;
        data._userId = q.value("userId").toUInt();
        data._objectTable = q.value("objectTable").toString();
        data._idObject = q.value("objectId").toUInt();
        data._lvl = static_cast<Permission>(q.value("lvl").toInt());

        _data.insert(data, data._lvl);
    }

    return isValid();
}

bool UserPermision::save(QSqlQuery &q) {
    QString queryString = "INSERT INTO %0(%1) VALUES ";

    queryString = queryString.arg(tableName());

    queryString = queryString.arg(
                "userId, objectTable, objectId, lvl");

    for (auto it = _data.begin(); it != _data.end(); ++it) {
        if (it != _data.begin()) {
            queryString += ",";
        }

        QString valuesLine = "(";
        valuesLine += "'" + QString::number(it.key()._userId) + "', ";
        valuesLine += "'" + it.key()._objectTable + "', ";
        valuesLine += "'" + QString::number(it.key()._idObject) + "', ";
        valuesLine += "'" + QString::number(static_cast<int>(it.value())) + "', ";
        valuesLine += ")";

        queryString += valuesLine;
    }


    if (!q.exec()) {
        QuasarAppUtils::Params::log("exec sql query error: " + q.lastError().text() + " Driver: " + q.lastError().driverText(),
                                    QuasarAppUtils::Debug);
        return false;
    }

    return true;
}

QDataStream &UserPermision::fromStream(QDataStream &stream) {
    return stream;
}

QDataStream &UserPermision::toStream(QDataStream &stream) const {
    return stream;
}

QHash<PermisionData, Permission> UserPermision::getData() const
{
    return _data;
}

void UserPermision::setData(const QHash<PermisionData, Permission> &data)
{
    _data = data;
}

void UserPermision::insertData(const QHash<PermisionData, Permission> &data) {
    for ( auto it = data.begin(); it != data.end(); it++) {
        _data.insert(it.key(), it.value());
    }
}

void UserPermision::removeData(const QHash<PermisionData, Permission> &data) {
    for ( auto it = data.begin(); it != data.end(); it++) {
        _data.remove(it.key());
    }
}

uint qHash(const NP::PermisionData &userPermision) {

    QByteArray data;
    QDataStream stream(&data, QIODevice::WriteOnly);

    stream << userPermision._userId;
    stream << userPermision._objectTable;
    stream << userPermision._idObject;
    return qHash(data);
}

bool operator ==(const PermisionData &left, const PermisionData &right) {
    return left._userId == right._userId &&
            left._idObject == right._idObject &&
            left._objectTable == right._objectTable;
}

}
