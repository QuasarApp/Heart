#ifndef NETWORKCLASSSIZE_H
#define NETWORKCLASSSIZE_H

#include "clientprotocol_global.h"

namespace ClientProtocol {
struct CLIENTPROTOCOLSHARED_EXPORT NetworkClassSize {
    unsigned int min = 0;
    unsigned int max = 0;

    bool isStaticType();

    NetworkClassSize(unsigned int size);
    NetworkClassSize(unsigned int min, unsigned int max);

    NetworkClassSize& operator = (unsigned int size);
    NetworkClassSize& operator + (const NetworkClassSize& );

    bool isValid(unsigned int size) const;
};
}



#endif // NETWORKCLASSSIZE_H
