#include "sign.h"
namespace QH {

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
