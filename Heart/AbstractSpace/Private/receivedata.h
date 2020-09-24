#ifndef RECEIVEDATA_H
#define RECEIVEDATA_H

#include "package.h"

namespace QH {

/**
 * @brief The ReceiveData struct This is private structure for contains informaton for parse packages
 */
struct ReceiveData
{
public:
    ReceiveData();

    Package _pkg;
    QByteArray _hdrArray;

};
}
#endif // RECEIVEDATA_H
