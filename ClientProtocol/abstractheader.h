#ifndef ABSTRACTHEADER_H
#define ABSTRACTHEADER_H
#include "clientprotocol_global.h"

namespace ClientProtocol {
/**
 * @brief The Header struct 4 byte
 */
#pragma pack(push, 1)
struct CLIENTPROTOCOLSHARED_EXPORT AbstractHeader {
    /**
     * @brief size - size of package data (not header)
     */
    unsigned short size;

    /**
     * @brief command of pacage
     */
    quint8 command;

    /**
     * @brief command of pacage see Command (rquest from client)
     * the server should write to which command it responds
     */
    quint8 triggerCommnad;

    /**
     * @brief Header default constructor
     */
    AbstractHeader();
    ~AbstractHeader() = default;

    /**
     * @brief isValid
     * @return true if header is valid
     */
    bool isValid() const;


    /**
     * @brief reset - reset all data and set for header invalid status
     */
    void reset();
};
#pragma pack(pop)

}



#endif // ABSTRACTHEADER_H
