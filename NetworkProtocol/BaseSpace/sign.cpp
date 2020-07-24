#include "sign.h"
namespace NP {

Sign::Sign()
{

}

QByteArray Sign::sign() const {
    return _sign;
}

void Sign::setSign(const QByteArray &sign) {
    _sign = sign;
}
}
