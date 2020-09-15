#ifndef SIGN_H
#define SIGN_H

#include <QByteArray>
#include "heart_global.h"


namespace QH {

/**
 * @brief The Sign class - this class contains sign of child data package.
 */
class NETWORKPROTOCOLSHARED_EXPORT Sign
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
