#ifndef RECEIVEDATA_H
#define RECEIVEDATA_H

#include "package.h"

namespace NP {

/**
 * @brief The ReceiveData struct - this structure contains informaton for parse packages
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
