/*
 * Copyright (C) 2023-2023 QuasarApp.
 * Distributed under the GPLv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
*/

#ifndef UNIVERSALDATA_H
#define UNIVERSALDATA_H

#include "abstractdata.h"

namespace QH {
namespace PKG {

/**
 * @brief The UniversalData class contains all data of object in the QMap
 * Use this package if you want to change your packages in prodaction.
 *
 * **Example:**
 *
 * @code{cpp}
 * class AuthRequest: public QH::PKG::UniversalData
    {
        QH_PACKAGE(RC::API::V4::AuthRequest)

        enum Filds{
            UserId = 0
        };

    public:
        AuthRequest();
        bool isValid() const override;

        QByteArray userId() const;
        void setUserId(const QByteArray &newUserId);
    };


    QByteArray AuthRequest::userId() const {
        return value(UserId).toByteArray();
    }

    void AuthRequest::setUserId(const QByteArray &newUserId) {
        setValue(UserId, newUserId);
    }
 * @endcode
 *
 */
class HEARTSHARED_EXPORT UniversalData: public QH::PKG::AbstractData
{
public:
    UniversalData();

    /**
     * @brief setValue This method sets new value for the filed. Works like a insert method of the QMap
     * @param key This is key of the filed.
     * @param value This is value of the key.
     */
    void setValue(int key, const QVariant& value);

    /**
     * @brief value this method return value of the key.
     * @return value of the key.
    */
    const QVariant& value(int key, const QVariant& defaultVal = {}) const;

    /**
     * @brief ref This method return pointer to the object If this object is not exists return nullptr
     * @param key This is key of object
     * @return pointer to the required object.
     */
    QVariant* ref(int key);

protected:
    QDataStream &fromStream(QDataStream &stream) override final;
    QDataStream &toStream(QDataStream &stream) const override final;

private:
    QHash<int, QVariant> _data;
};

}
}

#endif // UNIVERSALDATA_H
