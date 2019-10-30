#ifndef USERDATA_H
#define USERDATA_H

#include "dbobject.h"

namespace ClientProtocol {

/**
 * @brief The UserData class - is default implementation of users items of database
 */
class CLIENTPROTOCOLSHARED_EXPORT UserData : public DBObject
{
public:
    UserData();
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
    void setExtraData(const QVariantMap &extraData);

protected:
    QString _name;
    QString _passSHA256;
    QString _mail;
    int _lastOnline; // unix time
    int _onlineTime; // unix time
    QVariantMap _extraData;

    // StreamBase interface
    QDataStream &fromStream(QDataStream &stream) override;
    QDataStream &toStream(QDataStream &stream) const override;
    QVariantMap &fromVariantMap(QVariantMap &map) override;
    QVariantMap &toVariantMap(QVariantMap &map) const override;

    void clear() override;
    bool isValid() const override;
};

}

#endif // USERDATA_H
