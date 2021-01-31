/*
 * Copyright (C) 2018-2021 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
*/


#ifndef SIGN_H
#define SIGN_H

#include <QByteArray>
#include "heart_global.h"


namespace QH {

/**
 * @brief The Sign class - this class contains sign of child data package.
 */
class HEARTSHARED_EXPORT Sign
{
public:
    Sign();

    QByteArray sign() const;
    void setSign(const QByteArray &sign);

    QByteArray& sign();

    /**
     * @brief dataForSigned - thism metho return byte array for signed
     * @return data array
     */
    virtual QByteArray dataForSigned() const = 0;

private:
    QByteArray _sign;
};
}
#endif // SIGN_H
