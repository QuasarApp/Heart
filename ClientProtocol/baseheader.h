#ifndef BASEHEADER_H
#define BASEHEADER_H

#include "abstractheader.h"


namespace ClientProtocol {
struct CLIENTPROTOCOLSHARED_EXPORT BaseHeader : public AbstractHeader {
    /**
     * @brief Header default constructor
     */
    BaseHeader();
    ~BaseHeader() = default;
};
}



#endif // BASEHEADER_H
