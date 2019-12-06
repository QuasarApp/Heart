#ifndef USERDATA_H
#define USERDATA_H

#include "dbobject.h"

#include <accesstoken.h>

namespace NP {

/**
 * @brief The UserData class - is default implementation of users items of database
 */
class NETWORKPROTOCOLSHARED_EXPORT UserData : public DBObject
{
public:
    UserData();
    UserData(const Package& package);

    ~UserData() override;
    QString name() const;
    void setName(const QString &name);

    QString passSHA256() const;
    void setPassSHA256(const QString &passSHA256);

    QString mail() const;
    void setMail(const QString &mail);

    int lastOnline() const;
    void setLastOnline(int lastOnline);

    int onlineTime() const;
    void setOnlineTime(int onlineTime);

    QVariantMap extraData() const;
    QVariantMap& extraData();

    void setExtraData(const QVariantMap &extraData);

    // StreamBase interface
    QDataStream &fromStream(QDataStream &stream) override;
    QDataStream &toStream(QDataStream &stream) const override;

    bool select(QSqlQuery &q) override;
    bool save(QSqlQuery &q) override;
    bool remove(QSqlQuery &q) override;

    void clear() override;
    bool isValid() const override;
    SP<DBObject> factory() override;


    const AccessToken& token() const;
    void setToken(const AccessToken &token);
    bool copyFrom(const AbstractData *) override;

    int points() const;
    void setPoints(int points);

protected:
    QString _name;
    QString _passSHA256;
    QString _mail;
    int _lastOnline; // unix time
    int _onlineTime; // unix time
    int _points;

    QVariantMap _extraData;
    AccessToken _token;


};

}

#endif // USERDATA_H
