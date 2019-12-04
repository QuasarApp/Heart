#ifndef USERRECORD_H
#define USERRECORD_H

#include "abstractdata.h"


namespace NetworkProtocol {

/**
 * @brief The UserRecord class
 * This is simple class for viewing minimal user information.
 */
class NETWORKPROTOCOLSHARED_EXPORT UserRecord : public AbstractData
{
public:
    UserRecord();
    UserRecord(const Package& package);

    QDataStream &fromStream(QDataStream &stream);
    QDataStream &toStream(QDataStream &stream) const;

    bool isValid() const;

    QString getName() const;
    void setName(const QString &value);

    short getRecord() const;
    void setRecord(short value);

private:
    QString name;
    short record;
};

}

#endif // USERRECORD_H
