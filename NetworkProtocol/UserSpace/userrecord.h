/*
 * Copyright (C) 2018-2020 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
*/

#ifndef USERRECORD_H
#define USERRECORD_H

#include "abstractdata.h"


namespace NP {

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
