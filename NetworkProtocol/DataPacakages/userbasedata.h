#ifndef USERBASEDATA_H
#define USERBASEDATA_H

#include "dbobject.h"

namespace NP {

/**
 * @brief The UserBaseData class
 */
class NETWORKPROTOCOLSHARED_EXPORT UserBaseData: public DBObject
{
public:
    UserBaseData();

    // DBObject interface
public:
    QSharedPointer<DBObject> factory() override;
    bool select(QSqlQuery &q) override;
    bool save(QSqlQuery &q) override;

    QString name() const;
    void setName(const QString &name);

    QByteArray passSHA256() const;
    void setPassSHA256(const QByteArray &passSHA256);
    bool copyFrom(const AbstractData *) override;

    void clear() override;
    bool remove(QSqlQuery &q) override;
protected:
    QDataStream &fromStream(QDataStream &stream) override ;
    QDataStream &toStream(QDataStream &stream) const override;
private:
    QString _name;
    QByteArray _passSHA256;
};
}
#endif // USERBASEDATA_H
